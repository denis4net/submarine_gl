#ifndef ROCK_H
#define ROCK_H

#include "drawable.h"
#define ROCK_MODEL_FILE "resources/rock.obj"

class Rock : public Drawable
{
public:
    Rock();
public:
    void tick();
};

#endif // ROCK_H
