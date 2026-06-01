#include <ncurses.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include "main.h"

using namespace std;

int main() 
{
    initscr(); 
    cbreak(); 
    noecho();
    keypad(stdscr, TRUE);
    
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);   
    init_pair(2, COLOR_WHITE, COLOR_BLACK);  
    
    string currentDir;
    char* homePath = getenv("HOME");
    if (homePath != NULL)
    {
        currentDir = string(homePath);
    }
    else 
    {
        currentDir = "/";
    }

    vector<FileItem> files = enterfiles(currentDir);
    size_t selected = 0; 
    int ch = 0;
    bool showPermissions = false;
    bool showSize = false;

    while(true) 
    {
        clear();
        mvprintw(0, 0, "current Directory: %s", currentDir.c_str());
        
        for(size_t i = 0; i < files.size(); i++) 
        {
            int display_line = i + 2;
            
            if (files[i].isDirectory) 
            {
                attron(COLOR_PAIR(1));
            }
            else 
            {
                attron(COLOR_PAIR(2));
            }
            
            string prefix = (i == selected) ? "> " : "  ";

            if (showSize && showPermissions) 
            {
                mvprintw(display_line, 0, "%s%-40s %10s  %s", 
                         prefix.c_str(), files[i].name.c_str(), files[i].sizeStr.c_str(), files[i].permissions.c_str());
            } 
            else if (showSize && !showPermissions) 
            {
                mvprintw(display_line, 0, "%s%-40s %10s", 
                         prefix.c_str(), files[i].name.c_str(), files[i].sizeStr.c_str());
            } 
            else if (!showSize && showPermissions) 
            {
                mvprintw(display_line, 0, "%s%-40s %s", 
                         prefix.c_str(), files[i].name.c_str(), files[i].permissions.c_str());
            } 
            else 
            {
                mvprintw(display_line, 0, "%s%s", 
                         prefix.c_str(), files[i].name.c_str());
            }
            
            if (files[i].isDirectory) 
            {
                attroff(COLOR_PAIR(1));
            }
            else 
            {
                attroff(COLOR_PAIR(2));
            }
        }
        refresh();
        
        ch = getch();
        if (ch == 'q') 
        {
            break; 
        }
        
        if (ch == KEY_DOWN && selected < files.size() - 1) 
        {
            selected++;
        }
        if (ch == KEY_UP && selected > 0) 
        {
            selected--;
        }
        
        if (ch == '\n' || ch == KEY_RIGHT) 
        {
            if (files.empty()) 
            {
                continue;
            }
            
            string fullPath = currentDir + "/" + files[selected].name;
            if (files[selected].isDirectory) 
            {
                currentDir = fullPath;
                files = enterfiles(currentDir);
                selected = 0; 
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
        if(ch == 's')
        {
            showSize = !showSize;
        }
        
        if(ch == 'd')
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
                    if (selected >= files.size() && selected > 0)
                    {
                        selected--;
                    }
                }
            }
        }
        
        if(ch == 'c')
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
        
        if(ch == 'm')
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
    }
    
    endwin(); 
    return 0;
}
