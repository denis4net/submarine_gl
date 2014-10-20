#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "drawable.h"
#include "engine.h"

#define WATER_TEXTURE_FILE "resources/env.jpg"
#define BACKGROUND_SPHERE_RADIUS 5.0f

class Background: public Drawable
{
protected:
    GLUquadric* _quadric;
public:
    Background();

    virtual ~Background();

    void draw();
    void tick();
};

#endif // BACKGROUND_H
