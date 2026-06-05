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
    WINDOW* previewPane = allocatePreviewWindow();
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
    int selected = 0; 
    int ch = 0;
    bool showPermissions = false;
    bool showSize = false;

    while(true) 
    {
        clear();
        mvprintw(0, 9, "current Directory: %s", currentDir.c_str());
        
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
            
            string prefix = ((int)i == selected) ? "> " : "  ";
            
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

        previewFile(files, selected, previewPane, currentDir);
        refresh();
        
        ch = getch();
        if (ch == 'q') 
        {
            break;
        }
        handleKeybinds(ch, selected, files, currentDir, showPermissions, showSize);
    }
    delwin(previewPane); 
    endwin(); 
    return 0;
}
