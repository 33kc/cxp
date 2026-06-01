#ifndef FUNCTIONS
#define FUNCTIONS

#include <vector>
#include <string>
#include <sys/stat.h>
using namespace std;

struct FileItem {
  string name;
  bool isDirectory;
  string permissions;
};

vector<FileItem> enterfiles(string path);
bool isDir(string path);
string getPermissions(struct stat info);

#endif
