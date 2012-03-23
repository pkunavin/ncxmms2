#ifndef WINDOW_P_H
#define WINDOW_P_H

#include <vector>
#include "window.h"

namespace ncxmms2 {

class WindowPrivate
{
public:
    WindowPrivate(int lines_, int cols_, int yPos_, int xPos_, Window *parent_) :
        parent(parent_),
        focusedWindow(NULL),
        lines(lines_),
        cols(cols_),
        yPos(yPos_),
        xPos(xPos_),
        isVisible(true) {}

    WINDOW *cursesWin;
    Window *parent;
    std::vector<Window*> childrenWins;
    Window *focusedWindow;

    int lines;
    int cols;

    int yPos;
    int xPos;

    bool isVisible;

    std::string title;
};
} // ncxmms2

#endif // WINDOW_P_H
