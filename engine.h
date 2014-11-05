#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library

#include "structs.h"

enum Colors { RedColor = 0xffff0000,
              GreenColor = 0xff00ff00,
              BlueColor = 0xff0000ff
            };

namespace Engine
{
    void init(int argc, char **argv,void (*drawCallback)());
    void loop();
    void fillRect(Rect rect, uint color = 0xFFFFFF);
    void fillParallepiped(Rect base, float height, uint color = 0x99FFFFFF);
    void fillCircle(Point center, Length radius, uint color = 0xFFFFFF);
    void drawLine(Point start, Point end, int width = 1, uint color = 0xFF000000);
    void setColor(uint color);
    void translate(Point position);
    int getWidht();
    int getHeight();

    void InitGL(int Width, int Height);
    void addKeyHandler(KeyHandler handler);
};

#endif // ENGINE_H
