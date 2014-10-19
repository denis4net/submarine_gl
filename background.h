#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "drawable.h"
#define WATER_TEXTURE_FILE "resources/env.jpg"

class Background: public Drawable
{
public:
    Background();

    // Drawable interface
public:
    void draw();
    void tick();
};

#endif // BACKGROUND_H
