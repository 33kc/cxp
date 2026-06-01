#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include "main.h"
using namespace std;

vector<string> enterfiles(string path)
{
    vector<string> files;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) return files;
    
    struct dirent *entry;
    
    while((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_name[0] == '.') continue;
        files.push_back(entry->d_name);
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
