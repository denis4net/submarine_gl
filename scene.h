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
    void water();
    void mouseMove(int x, int y);
    void mouseClick(int button, int state, int x, int y);
    void zoomIn();
    void zoomOut();
};

#endif // SCENE_H
