#ifndef FUNCTIONS
#define FUNCTIONS

#include <vector>
#include <string>
#include <sys/stat.h>
#include <ncurses.h>
using namespace std;

enum FileType {
  TYPE_DIR,
  TYPE_TEXT,
  TYPE_IMAGE,
  TYPE_UNKNOWN
};


struct FileItem {
  string name;
  bool isDirectory;
  string permissions;
  string sizeStr;
  FileType type;
};

vector<FileItem> enterfiles(string path);
string formatString(long long bytes);
bool isDir(string path);
string getPermissions(struct stat info);
int newFolder(string pathname, mode_t permissions);
WINDOW* allocatePreviewWindow();
void previewFile(vector<FileItem> files, int selected, WINDOW* previewWin, string currPath);
void handleKeybinds(int ch, int& selected, vector<FileItem>& files, string& currentDir, bool& showPermissions, bool& showSize);


#endif
