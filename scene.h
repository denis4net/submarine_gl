#ifndef SCENE_H
#define SCENE_H

#include "structs.h"
#include "engine.h"

namespace Scene
{
    void init();
    void drawBackground();
    void drawScene();
    void drawObjects();
    void launchRocket();
    void ticker();
    void up();
    void down();
};

#endif // SCENE_H
