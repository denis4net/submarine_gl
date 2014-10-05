#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library

#include "structs.h"

namespace Engine
{
    void init(int argc, char **argv,void (*drawCallback)());
    void loop();
    void fillRect(Rect rect, uint color = 0xFFFFFF);
    void fillParallepiped(Rect base, float height, uint color = 0x99FFFFFF);

    void fillCircle(Point center, Length radius, uint color = 0xFFFFFF);
    void InitGL(int Width, int Height);
    void addKeyHandler(KeyHandler handler);
};

#endif // ENGINE_H
