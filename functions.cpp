#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
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
        
        if (stat(fullPath.c_str(), &info) == 0)
        {
            isFolder = S_ISDIR(info.st_mode);
            permsStr = getPermissionsStr(info);
            
            if (!isFolder)
            {
                sizeString = formatSize(info.st_size);
            }
        }
        
        files.push_back({entry->d_name, isFolder, permsStr, sizeString});
    }
    
    closedir(dir);
    return files;
}


