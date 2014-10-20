#include "drawable.h"
#include "engine.h"
#include "utils.h"
#include "structs.h"

#include <GL/gl.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <SOIL/SOIL.h>

using namespace std;

Drawable::Drawable(const char *path) {

    loadFromFile(path);

    _xRot = 0;
    _yRot = 0;
    _zRot = 0;
}

bool Drawable::loadFromFile(const char *path)
{
    _scene = _importer.ReadFile( path,
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

    if (_scene == nullptr)
    {
        ERROR("Can't load model \"%s\": %s", path, _importer.GetErrorString());
        return false;
    }
    return true;
}

bool Drawable::loadTextures(const char *filename)
{
    _textureLoaded = true;
    /* load an image file directly as a new OpenGL texture */
    GLuint tex2D = SOIL_load_OGL_texture (
            filename,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    if (tex2D == 0)
    {
        ERROR("Texture \"%s\" load error: %s", filename, SOIL_last_result());
    }
    else
        _textures.push_back(tex2D);
}



void Drawable::setColor(unsigned int color)
{
    _color = color;
}

void Drawable::draw()
{

    glPushMatrix();
    glPushAttrib(~0);

    if (_blendingValue == 0xff)
        glDisable(GL_BLEND);
    else
        glEnable(GL_BLEND);

    glDisable(GL_CULL_FACE);

    Engine::setColor(_color);

    glTranslatef(_position.x, _position.y, _position.z);
    glRotatef(_xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(_yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(_zRot, 0.0f, 0.0f, 1.0f);


    GLfloat specular_color[] = { _specular, _specular, _specular, 0.9f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
    glMaterialfv(GL_FRONT, GL_SHININESS, &_shininess);

    glScalef(_scale, _scale, _scale);

    if (_textureLoaded)
         glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    for (size_t meshIndex = 0; meshIndex < _scene->mNumMeshes; meshIndex++)
    {
        aiMesh *mesh = _scene->mMeshes[meshIndex];
        for (size_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex ++)
        {
            aiFace face = mesh->mFaces[faceIndex];
            GLint type = (face.mNumIndices == 3) ? GL_TRIANGLES : GL_POLYGON;
            glBegin(type);

            for (size_t i = 0; i < face.mNumIndices; i++)
            {
                size_t vertexIndex = face.mIndices[i];

                if (_textureLoaded && mesh->mTextureCoords  && !_textures.empty())
                {
                    GLint tex = _textures.front();
                    glBindTexture(GL_TEXTURE_2D, tex);
                    glTexCoord2f(mesh->mTextureCoords[0]->x, mesh->mTextureCoords[0]->y);
                }
                glNormal3f(-mesh->mNormals[vertexIndex].x,
                           -mesh->mNormals[vertexIndex].y,
                           -mesh->mNormals[vertexIndex].z);

                glVertex3f(mesh->mVertices[vertexIndex].x,
                           mesh->mVertices[vertexIndex].y,
                           mesh->mVertices[vertexIndex].z
                           );
            }
            glEnd();
        }
    }

    glPopAttrib();
    glPopMatrix();
}

bool Drawable::isShouldBeDestroyed()
{
    return _shoudBeDestroyed;
}

Point &Drawable::getPosition()
{
    return _position;
}



Rocket::Rocket(const char *path): Drawable(path), ticks(0)
{
    setColor(0xFF111111);
    _scale = 0.015f;
}

void Rocket::tick()
{
    ticks += 1;

    getPosition().y += ROCKET_Y_SPEED;
    getPosition().x -= ROCKET_X_SPEED;
}



Submarine::Submarine(const char *path): Drawable(path), _R(1.3f), _angle(0.0f) {
    setColor(0xFF333333);
    setShininess(0.8f);
    _scale = 0.8f;
    setSpecular(0.8f);
    loadTextures("resources/submarine/texture.jpg");
    setShininess(100.0f);
    setSpecular(0.3f);
}

void Submarine::tick()
{
    _position.x = _R * sin(_angle);
    _position.z = _R * cos(_angle);

    _blendingValue = - _position.y;

    _yRot = -(270-(_angle*180/M_PI));
    _angle += 0.01;
    setShininess(128.0f);
    setSpecular(1.0f);
}

bool Submarine::isActivated()
{
    return _isActivated;
}

void Submarine::activate()
{
    _isActivated = true;
}
