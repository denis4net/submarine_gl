#include "engine.h"
#include "utils.h"
#include "structs.h"

#include <unistd.h>
#include <chrono>
#include <thread>
#include <list>
#include <cmath>


/* ASCII code for the escape key. */
#define ESCAPE 27

/* The number of our GLUT window */
static int window;
static void (*_drawCallback)();
static std::list<KeyHandler> _keyHandlers;

/* The function called whenever a key is pressed. */
static void keyPressed(unsigned char key, int x, int y)
{
    /* sleep to avoid thrashing this procedure */
    usleep(10);

    /* If escape is pressed, kill everything. */
    if (key == ESCAPE)
    {
        /* shut down our window */
        glutDestroyWindow(window);

        /* exit the program...normal termination. */
        exit(0);
    }

    for (const KeyHandler& h: _keyHandlers)
    {
        if (key == h.keycode)
        {
            h.fun();
        }
    }

}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
static void ReSizeGLScene(int Width, int Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
        Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}


void Engine::init(int argc, char **argv, void (*drawCallback)())
{
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
    _drawCallback = drawCallback;

    glutInit(&argc, argv);

    /* Select type of Display mode:
       Double buffer
       RGBA color
       Alpha components supporte
       Depth buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

    /* get a 640 x 480 window */
    glutInitWindowSize(640, 480);

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);

    /* Open a window */
    window = glutCreateWindow("Submarine project");

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(_drawCallback);

    /* Go fullscreen.  This is as soon as possible. */
    //glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(_drawCallback);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);

    /* Initialize our window. */
    InitGL(640, 480);
}


void Engine::loop()
{
    /* Start Event Processing Engine */
    glutMainLoop();
}


/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void Engine::InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);

    glClearDepth(1.0);                        // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                     // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);                  // Enables Depth Testing
    glShadeModel(GL_SMOOTH);                  // Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDepthFunc(GL_EQUAL);
}


void Engine::fillRect(Rect rect, uint color /* = 0xFFFFFF */)
{
    //LOG("Drawing rectangle: %f %f %f, w:%f, h: %f", rect.start.x, rect.start.y, rect.start.z, rect.width, rect.height);
    glColor4ub(0xff & (color >> 16), 0xff & (color >> 8), 0xff & color, 0xff & (color >> 24));

    glBegin(GL_QUADS);
    glVertex3f(rect.start.x, rect.start.y-rect.height, 0);
    glVertex3f(rect.start.x + rect.width, rect.start.y-rect.height, 0);
    glVertex3f(rect.start.x + rect.width, rect.start.y, 0);
    glVertex3f(rect.start.x, rect.start.y, 0);
    glEnd();
}

void Engine::fillCircle(Point center, Length radius, uint color /* = 0xFFFFFF */)
{
    //LOG("Drawing rectangle: %f %f %f, w:%f, h: %f", rect.start.x, rect.start.y, rect.start.z, rect.width, rect.height);
    glColor4ub(0xff & (color >> 16), 0xff & (color >> 8), 0xff & color, 0xff & (color >> 24));

    glBegin(GL_POLYGON);
    for(double a = 0; a <= M_PI * 2; a += 0.01f)
    {
        float x = radius * cos(a) + center.x;
        float y = radius * sin(a) + center.y;
        glVertex2f(x, y);
    }

    glEnd();
}


void Engine::addKeyHandler(KeyHandler handler)
{
    _keyHandlers.push_back(handler);
}


void Engine::fillParallepiped(Rect base, float height, uint color)
{

}
