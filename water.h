#ifndef WATER_H
#define WATER_H

#include "drawable.h"

#define DEFAULT_WATER_LEVEL 0.0f

class Water: public Drawable
{
protected:
    float z(const float x, const float y, const float t);
    int	loadTexture(const char * filename,
                      unsigned char * dest,
                      const int format,
                      const unsigned int size);
    int init();

public:
    Water();
    virtual void draw() override;
    virtual void tick() override;
};

#endif
