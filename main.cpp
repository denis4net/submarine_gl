#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library

#include <unistd.h>     // Header File for sleeping.
#include "structs.h"
#include "engine.h"
#include "scene.h"

int main(int argc, char **argv)
{
    Engine::init(argc, argv, Scene::drawScene);
    Scene::init();
    Engine::loop();
    return 0;
}
