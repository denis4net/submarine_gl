#ifndef COORDINATEAXIS_H
#define COORDINATEAXIS_H

#include "drawable.h"

class CoordinateAxis : public Drawable
{
public:
    CoordinateAxis();

    // Drawable interface
public:
    void draw();
    void tick();
};

#endif // COORDINATEAXIS_H
