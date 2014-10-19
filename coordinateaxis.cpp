#include "coordinateaxis.h"
#include "engine.h"

CoordinateAxis::CoordinateAxis(): Drawable({0, 0, 0})
{
}

void CoordinateAxis::draw()
{
    glPushMatrix();
    glPushAttrib(GL_BLEND | GL_TEXTURE_2D);
    Engine::drawLine({0,0,0}, {1,0,0}, 2, GreenColor);
    Engine::drawLine({0,0,0}, {0,1,0}, 2, RedColor);
    Engine::drawLine({0,0,0}, {0,0,1}, 2, BlueColor);
    glPopAttrib();
    glPopMatrix();
}

void CoordinateAxis::tick()
{

}
