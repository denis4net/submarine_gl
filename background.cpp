#include "background.h"
#include "engine.h"

Background::Background(): Drawable({0, 0, 0})
{
    loadTextures(WATER_TEXTURE_FILE);
    setColor(0xf0ffffff);
}

void Background::draw()
{
    glPushMatrix();
    glPushAttrib(~0);

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, _textures[0]);

    Engine::translate(getPosition());
    Engine::setColor(_color);

    GLUquadric* quadric = gluNewQuadric();
    if (quadric == nullptr)
        return;

    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluQuadricTexture(quadric, GL_TRUE);

    gluSphere(quadric, 5, 100, 100);

    glPopAttrib();
    glPopMatrix();
}

void Background::tick()
{

}
