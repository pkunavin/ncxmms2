#ifndef RECTANGLE_H
#define RECTANGLE_H

namespace ncxmms2 {

class Rectangle
{
public:
    Rectangle(int xPos, int yPos, int cols, int lines) :
        m_xPos(xPos), m_yPos(yPos), m_cols(cols), m_lines(lines) {}

    int xPos() const  {return m_xPos;}
    int yPos() const  {return m_yPos;}
    int cols() const  {return m_cols;}
    int lines() const {return m_lines;}

private:
    int m_xPos;
    int m_yPos;
    int m_cols;
    int m_lines;
};

} // ncxmms2

#endif // RECTANGLE_H
