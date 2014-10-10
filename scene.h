#ifndef SCENE_H
#define SCENE_H

#include "structs.h"
#include "engine.h"

namespace Scene
{
    void init();
    void drawBackground();
    void drawAxis();
    void drawScene();
    void drawObjects();
    void launchRocket();
    void ticker();
    void up();
    void down();
    void mouseMove(int x, int y);
};

#endif // SCENE_H
