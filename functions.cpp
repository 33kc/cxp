#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include "main.h"

using namespace std;

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
        bool isFolder = isDir(fullPath);
        
        files.push_back({entry->d_name, isFolder});
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
