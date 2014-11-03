#include "background.h"

Background::Background(): Drawable({0, -1.0f, 0})
{
    loadTextures(WATER_TEXTURE_FILE);
    setColor(0xf0ffffff);

    _quadric = gluNewQuadric();
    if (_quadric == nullptr)
        return;


    gluQuadricNormals(_quadric, GLU_FLAT);
    gluQuadricDrawStyle(_quadric, GLU_FILL);
    gluQuadricOrientation(_quadric, GLU_INSIDE);
    gluQuadricTexture(_quadric, GL_TRUE);

}

Background::~Background()
{
    if (_quadric != nullptr)
        gluDeleteQuadric(_quadric);
}

void Background::draw()
{
    glPushMatrix();
    glPushAttrib(~0);

    glRotatef(90, 1, 0, 0);
    Engine::translate(getPosition());
    Engine::setColor(_color);

    glEnable(GL_TEXTURE_2D);

    assert(!_textures.empty());

    glBindTexture(GL_TEXTURE_2D, _textures.front());

    gluSphere(_quadric, BACKGROUND_SPHERE_RADIUS, 100, 100);

    glPopAttrib();
    glPopMatrix();
}

void Background::tick()
{

}
