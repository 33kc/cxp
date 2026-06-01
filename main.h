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
  string sizeStr;
};

vector<FileItem> enterfiles(string path);
string formatString(long long bytes);
bool isDir(string path);
string getPermissions(struct stat info);


#endif
