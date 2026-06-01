#ifndef FUNCTIONS
#define FUNCTIONS

#include <vector>
#include <string>
using namespace std;

struct FileItem {
  string name;
  bool isDirectory;
};

vector<FileItem> enterfiles(string path);
bool isDir(string path);

#endif
