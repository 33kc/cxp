#include <ncurses.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <cstdlib>
#include "main.h"

using namespace std;

int main() 
{
    initscr(); cbreak(); noecho();
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
    else currentDir = "/";

    vector<FileItem> files = enterfiles(currentDir);
    size_t selected = 0; 
    int ch = 0;
    bool showPermissions=false;

    while(true) 
    {
        clear();
        
        mvprintw(0, 0, "current Directory: %s", currentDir.c_str());
        
        for(size_t i = 0; i < files.size(); i++) 
        {
            int display_line = i + 2;
            
            if (files[i].isDirectory) attron(COLOR_PAIR(1));
            else attron(COLOR_PAIR(2));
            
            if (showPermissions) 
            {
                if (i == selected)
                    mvprintw(display_line, 0, "> %-20s %s", files[i].name.c_str(), files[i].permissions.c_str());
                else
                    mvprintw(display_line, 0, "  %-20s %s", files[i].name.c_str(), files[i].permissions.c_str());
            }
            else 
            {
                if (i == selected)
                    mvprintw(display_line, 0, "> %s", files[i].name.c_str());
                else
                    mvprintw(display_line, 0, "  %s", files[i].name.c_str());
            }            
            if (files[i].isDirectory) attroff(COLOR_PAIR(1));
            else attroff(COLOR_PAIR(2));
        }
        refresh();
        
        ch = getch();
        if (ch == 'q') break; 
        
        if (ch == KEY_DOWN && selected < files.size() - 1) selected++;
        if (ch == KEY_UP && selected > 0) selected--;
        
        if (ch == '\n' || ch == KEY_RIGHT) 
        {
            if (files.empty()) continue;
            
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
            showPermissions=!showPermissions;
        }
    }
    
    endwin(); 
    return 0;
}
