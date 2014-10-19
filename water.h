#ifndef WATER_H
#define WATER_H

#include "drawable.h"

#define DEFAULT_WATER_LEVEL 0.0f
#define WATER_FOG_LEVEL -0.1f;
#define	RESOLUTION 64


class Water: public Drawable
{
protected:
    float z(const float x, const float y, const float t);
    int	loadTexture(const char * filename,
                      unsigned char * dest,
                      const int format,
                      const unsigned int size);
    int init();
    void generate();

    GLuint	_texture;
    unsigned int _length;

    float	_surface[6 * RESOLUTION * (RESOLUTION + 1)];
    float	_normal[6 * RESOLUTION * (RESOLUTION + 1)];

public:
    Water();
    virtual void draw() override;
    virtual void tick() override;
};

#endif
