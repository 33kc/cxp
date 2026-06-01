#include <ncurses.h>
#include <vector>
#include <string>
#include "main.h"
using namespace std;

int main() 
{
    initscr(); cbreak(); noecho();
    keypad(stdscr, TRUE);
    
    string currentDir = "/home/kc";
    vector<string> files = enterfiles(currentDir);
    int selected = 0, ch = 0;
    
    while(true) 
    {
        clear();
        for(size_t i = 0; i < files.size(); i++) 
        {
            if ((int)i == selected) mvprintw(i, 0, "> %s", files[i].c_str());
            else mvprintw(i, 0, "  %s", files[i].c_str());
        }
        refresh();
        
        ch = getch();
        if (ch == 'q') break;
        if (ch == KEY_DOWN && selected < (int)files.size() - 1) selected++;
        if (ch == KEY_UP && selected > 0) selected--;
        
        if (ch == '\n' || ch == KEY_RIGHT) 
        {
            if (files.empty()) continue;
            string fullPath = currentDir + "/" + files[selected];
            if (isDir(fullPath)) 
            {
                currentDir = fullPath;
                files = enterfiles(currentDir);
                selected = 0;
            }
        }
    }
    
    endwin();
    return 0;
}
