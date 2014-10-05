#include "drawable.h"
#include "engine.h"



Drawable::Drawable(Point p): position(p), shoudBeDestroyed(false)
{}

bool Drawable::isShouldBeDestroyed()
{
    return shoudBeDestroyed;
}

Point Drawable::getPosition()
{
    return position;
}


Rocket::Rocket(Point p): Drawable(p), ticks(0)
{}

void Rocket::draw()
{
    Engine::fillRect({{position.x, position.y, position.z}, 0.03f, 0.06f}, COLOR_ROCKET);
    Point rocketHeadPos = position;
    rocketHeadPos.x += 0.015f;
    Engine::fillCircle(rocketHeadPos, 0.015f, COLOR_ROCKET);
}

void Rocket::tick()
{
        ticks += 1;

        position.y += ROCKET_Y_SPEED;
        position.x -= ROCKET_X_SPEED;
}


Submarine::Submarine(Point p): Drawable(p), _isActivated(false)
{

}

void Submarine::draw()
{
    Engine::fillRect({{position.x, position.y, position.z}, 0.3f, 0.1f}, COLOR_SUBMARINE);
    Point circlePos = position;
    circlePos.y -= 0.05f;
    Engine::fillCircle(circlePos, 0.05f, COLOR_SUBMARINE);
    circlePos.x += 0.3f;
    Engine::fillCircle(circlePos, 0.05f, COLOR_SUBMARINE);
    circlePos.y += 0.05f;
    circlePos.x -= 0.1f;
    Engine::fillCircle(circlePos, 0.05f, COLOR_SUBMARINE);

}


void Submarine::tick()
{
    position.x =  position.x - SUBMARINE_GAP > -1.0f ?
                position.x - SUBMARINE_GAP : 1.0f;
}

bool Submarine::isActivated()
{
    return _isActivated;
}

void Submarine::activate()
{
    _isActivated = true;
}
