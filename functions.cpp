#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "main.h"
using namespace std;

bool isDir(string path)
{
    struct stat info;
    if (stat(path.c_str(), &info) == 0)
    {
        return S_ISDIR(info.st_mode);
    }
    return false;
}

string getPermissionsStr(struct stat info)
{
    string perms = "---------";
    
    if (S_ISDIR(info.st_mode)) perms[0] = 'd';
    
    if (info.st_mode & S_IRUSR) perms[0] = 'r';
    if (info.st_mode & S_IWUSR) perms[1] = 'w';
    if (info.st_mode & S_IXUSR) perms[2] = 'x';
    
    if (info.st_mode & S_IRGRP) perms[3] = 'r';
    if (info.st_mode & S_IWGRP) perms[4] = 'w';
    if (info.st_mode & S_IXGRP) perms[5] = 'x';
    
    if (info.st_mode & S_IROTH) perms[6] = 'r';
    if (info.st_mode & S_IWOTH) perms[7] = 'w';
    if (info.st_mode & S_IXOTH) perms[8] = 'x';
    
    return perms;
}

string formatSize(long long bytes)
{
    double size = bytes;
    int unit = 0;
    string units[] = {"B", "KB", "MB", "GB"};
    
    while (size >= 1024 && unit < 3)
    {
        size /= 1024.0;
        unit++;
    }
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.1f %s", size, units[unit].c_str());
    return string(buffer);
}

vector<FileItem> enterfiles(string path)
{
    vector<FileItem> files;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL)
    {
        return files;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        
        string fullPath = path + "/" + entry->d_name;
        struct stat info;
        string permsStr = "---------";
        string sizeString = "-";
        bool isFolder = false;
        
        FileType type = TYPE_UNKNOWN; 
        
        vector<string> iExp = {".png", ".jpeg", ".jpg"}; // add more suffix extensions later
        vector<string> tExp = {".txt", ".md", ".cpp", ".h", ".json"};

        if (stat(fullPath.c_str(), &info) == 0)
        {
            isFolder = S_ISDIR(info.st_mode);
            permsStr = getPermissionsStr(info);
            
            if (!isFolder)
            {
                sizeString = formatSize(info.st_size);
                
                string filename = entry->d_name;
                int last = filename.find_last_of(".");
                
                if (last != string::npos)
                {
                    string extension = filename.substr(last);
                   
                    for (int i = 0; i < tExp.size(); i++)
                    {
                        if (extension == tExp[i])
                        {
                            type = TYPE_TEXT;
                            break;
                        }
                    }
                    
                    if (type == TYPE_UNKNOWN)
                    {
                        for (int i = 0; i < iExp.size(); i++)
                        {
                            if (extension == iExp[i])
                            {
                                type = TYPE_IMAGE;
                                break;
                            }
                        }
                    }
                }
            }
            else 
            {
                type = TYPE_DIR;
            }
        }
        
        files.push_back({entry->d_name, isFolder, permsStr, sizeString, type});
    }
    
    closedir(dir);
    return files;
}

int newFolder(string pathname, mode_t permissions)
{
   return mkdir(pathname.c_str(), permissions); 
}


WINDOW* allocatePreviewWindow()
{
    int parentY, parentX;
    getmaxyx(stdscr, parentY, parentX);
    int leftCols = parentX * 0.55;       
    int previewCols = parentX - leftCols;
    
    int startY = 1;
    int startX = leftCols;
    int height = parentY - 1;
    WINDOW* previewWin = derwin(stdscr, height, previewCols, startY, startX);
    
    return previewWin;
}


void previewFile(vector<FileItem> file, int selected, WINDOW* previewWin, string currentPath)
{
   wclear(previewWin);
   int type = file[selected].type;
   
   if(type == TYPE_DIR) mvwprintw(previewWin, 0, 0, "directory, nothing to preview");
   else if (type == TYPE_TEXT)
   {  
      mvwprintw(previewWin, 0, 0, "File: %s", file[selected].name.c_str());
      
      string fullPath = currentPath + "/" + file[selected].name;
      ifstream text(fullPath);
      
      if(text.is_open())
      {
         string line = "";
         int row = 2;
         int maxY = getmaxy(previewWin);

         while(getline(text, line) && row < maxY - 1)
         {
            mvwprintw(previewWin, row, 0, "%s", line.c_str());
            row++;
         }
         text.close();
      }
      else mvwprintw(previewWin, 2, 0, "Error, cant open file");
   }
   else mvwprintw(previewWin, 0, 0, "not a file format that's previewable");
   
   wrefresh(previewWin);
}

void handleKeybinds(int ch, int& selected, vector<FileItem>& files, string& currentDir, bool& showPermissions, bool& showSize)
{
    if (ch == KEY_DOWN && selected < (int)files.size() - 1) 
    {
        selected++;
    }
    if (ch == KEY_UP && selected > 0) 
    {
        selected--;
    }
    
    if (ch == '\n' || ch == KEY_RIGHT) 
    {
        if (!files.empty()) 
        {
            string fullPath = currentDir + "/" + files[selected].name;
            if (files[selected].isDirectory) 
            {
                currentDir = fullPath;
                files = enterfiles(currentDir);
                selected = 0; 
            }
            else if (files[selected].type == TYPE_IMAGE)
            {
                pid_t pid = fork();
                if (pid == 0)
                {
                    int devNull = open("/dev/null", O_WRONLY);
                    dup2(devNull, STDERR_FILENO);
                    close(devNull);
                    execlp("feh", "feh", "--scale-down", fullPath.c_str(), (char*)NULL);
                    _exit(1);
                }
            }
        }
    }
    
    if (ch == 'u' || ch == KEY_LEFT) 
    {
        string path = currentDir;
        size_t point = path.find_last_of('/');
        
        if (point != string::npos && point > 0) 
        {
            path.erase(point);
            currentDir = path;
            files = enterfiles(currentDir);
            selected = 0;
        }
    }
    
    if (ch == 'p')
    {
        showPermissions = !showPermissions;
    }
    if (ch == 's')
    {
        showSize = !showSize;
    }
    
    if (ch == 'd')
    {
        if (!files.empty())
        {
            move(0, 0);
            clrtoeol();
            mvprintw(0, 0, "Are you sure you want to delete %s? (y/n): ", files[selected].name.c_str());
            refresh();
            
            char choice = getch();
            if (choice == 'y' || choice == 'Y')
            {
                string fullPath = currentDir + "/" + files[selected].name;
                remove(fullPath.c_str());
                
                files = enterfiles(currentDir);
                if (selected >= (int)files.size() && selected > 0)
                {
                    selected--;
                }
            }
        }
    }
    
    if (ch == 'c')
    {
        if (!files.empty())
        {
            move(0, 0);
            clrtoeol();
            mvprintw(0, 0, "Copy destination path/name: ");
            refresh();
            
            echo();
            char destinationPath[256];
            getstr(destinationPath);
            noecho();
            
            string source = currentDir + "/" + files[selected].name;
            string command = "cp \"" + source + "\" \"" + string(destinationPath) + "\"";
            system(command.c_str());
            
            files = enterfiles(currentDir);
        }
    }
    
    if (ch == 'm')
    {
        if (!files.empty())
        {
            move(0, 0);
            clrtoeol();
            mvprintw(0, 0, "Enter new name/destination path for %s: ", files[selected].name.c_str());
            refresh();
            
            echo();
            char buffer[256];
            getstr(buffer);
            noecho();
            
            string userInput = string(buffer);
            string newPath = "";
            bool isPlainName = true;
            
            for (size_t i = 0; i < userInput.size(); i++)
            {
                if (userInput[i] == '/')
                {
                    isPlainName = false;
                    break;
                }
            }
            
            if (isPlainName)
            {
                newPath = currentDir + "/" + userInput;
            }
            else 
            {
                newPath = userInput;
            }
            
            string oldPath = currentDir + "/" + files[selected].name;
            rename(oldPath.c_str(), newPath.c_str());
            
            files = enterfiles(currentDir);
        }
    }
    if (ch == 'a')
    {
        move(0, 0);
        clrtoeol();
        mvprintw(0, 0, "Enter name for new folder: ");
        refresh();
        
        echo();
        char buffer[256];
        getstr(buffer);
        noecho();
        
        string userInput = string(buffer);
        if (!userInput.empty())
        {
            string newFolderPath = currentDir + "/" + userInput;
            newFolder(newFolderPath.c_str(), 0777);
            
            files = enterfiles(currentDir);
        }
    }
}
