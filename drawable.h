#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "structs.h"

#define COLOR_BACKGROUND 0xAEF6FF
#define COLOR_SUBMARINE 0x999999
#define COLOR_ROCKET 0x222222
#define SUBMARINE_GAP 0.01f

#define WATER_LINE_LEVEL 0.0f

#define ROCKET_Y_SPEED 0.04f
#define ROCKET_X_SPEED 0.003f

class Drawable
{
protected:

    bool shoudBeDestroyed;
public:
    Point position;

    Drawable(Point p = {0.0f, 0.0f, 0.0f});
    virtual void draw() = 0;
    virtual void tick() = 0;
    virtual bool isShouldBeDestroyed();
    virtual Point getPosition();
};

struct Rocket : public Drawable
{
private:
    AngleRad direction;
    unsigned int ticks;
public:
    Rocket(Point p = {0.0f, 0.0f, 0.0f});
    virtual void draw() override;
    virtual void tick() override;
};

struct Submarine: public Drawable
{
    bool _isActivated;
    Submarine(Point p = {0.0f, 0.0f, 0.0f});
public:
    virtual void draw() override;
    virtual void tick() override;

    virtual bool isActivated();
    virtual void activate();
};

#endif // DRAWABLE_H
