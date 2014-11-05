#ifndef SCENE_H
#define SCENE_H

#include "structs.h"
#include "drawable.h"
#include "engine.h"

namespace Scene
{
    void init();
    void setLighting();
    void drawAxis();
    void render();
    void drawObjects();
    void launchRocket();
    void ticker();
    void up();
    void down();
    void water();
    void fog();

    void mouseMove(int x, int y);
    void mouseClick(int button, int state, int x, int y);
    void zoomIn();
    void zoomOut();

    void reshape(int w, int h);
    void renderScene();
    void initShadow();
    void renderToShadowMap();

    void renderPlane();
    void renderCube();
};

#endif // SCENE_H
