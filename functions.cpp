#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include "main.h"

using namespace std;

string getPermissionsStr(struct stat info) 
{
    string perms = "";
    
    perms += (S_ISDIR(info.st_mode)) ? "d" : "-";
    
    perms += (info.st_mode & S_IRUSR) ? "r" : "-";
    perms += (info.st_mode & S_IWUSR) ? "w" : "-";
    perms += (info.st_mode & S_IXUSR) ? "x" : "-";
    
    perms += (info.st_mode & S_IRGRP) ? "r" : "-";
    perms += (info.st_mode & S_IWGRP) ? "w" : "-";
    perms += (info.st_mode & S_IXGRP) ? "x" : "-";
    
    perms += (info.st_mode & S_IROTH) ? "r" : "-";
    perms += (info.st_mode & S_IWOTH) ? "w" : "-";
    perms += (info.st_mode & S_IXOTH) ? "x" : "-";
    
    return perms;
}


vector<FileItem> enterfiles(string path) 
{
    vector<FileItem> files;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) return files;
    
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_name[0] == '.') continue;
        
        string fullPath = path + "/" + entry->d_name;
        
        struct stat info;
        string permsStr = "----------"; 
        bool isFolder = false;
        
        if (stat(fullPath.c_str(), &info) == 0) 
        {
            isFolder = S_ISDIR(info.st_mode);
            permsStr = getPermissionsStr(info); 
        }
        files.push_back({entry->d_name, isFolder, permsStr});
    }
    closedir(dir);
    return files;
}

bool isDir(string path) 
{
    struct stat info;
    if (stat(path.c_str(), &info) == -1) return false;
    return S_ISDIR(info.st_mode);
}
