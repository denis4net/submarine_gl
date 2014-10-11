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


bool Drawable::loadFromFile(const char *path)
{
    _scene = _importer.ReadFile( path,
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

    if (_scene == nullptr)
    {
        ERROR("Can't load modle \"%s\": %s", path, _importer.GetErrorString());
        return false;
    }
    return true;
}

bool Drawable::loadTextures(const char *filename)
{
    /* load an image file directly as a new OpenGL texture */
    GLuint tex2D = SOIL_load_OGL_texture (
            filename,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    if (tex2D == 0)
    {
        ERROR("Texture \"%s\" load error", filename);
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

    bool blendEnabled = glIsEnabled(GL_BLEND);
    glDisable(GL_BLEND);


    glColor4ub(0xff & (_color >> 16), 0xff & (_color >> 8), 0xff & _color, 0xff & (_color >> 24));

    glTranslatef(_position.x, _position.y, _position.z);
    glRotatef(_xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(_yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(_zRot, 0.0f, 0.0f, 1.0f);

    //glMaterialfv(GL_FRONT, GL_SPECULAR, _materialSpecular);
    glScalef(_scale, _scale, _scale);

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

                if (mesh->mTextureCoords  && !_textures.empty())
                {
                    GLint tex = _textures.front();

                    glBindTexture(GL_TEXTURE_2D, tex);

                    glTexCoord2f(
                                mesh->mTextureCoords[0]->x,
                                mesh->mTextureCoords[0]->y
                            );
                }
                else
                    glDisable (GL_TEXTURE_2D);

                glNormal3f( mesh->mNormals[vertexIndex].x,
                            mesh->mNormals[vertexIndex].y,
                            mesh->mNormals[vertexIndex].z);

                glVertex3f(mesh->mVertices[vertexIndex].x,
                           mesh->mVertices[vertexIndex].y,
                           mesh->mVertices[vertexIndex].z
                           );
            }
            glEnd();
        }
    }
    if (blendEnabled)
        glEnable(GL_BLEND);

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
    _scale = 0.01f;
}

void Rocket::tick()
{
    ticks += 1;

    _position.y += ROCKET_Y_SPEED;
    _position.x -= ROCKET_X_SPEED;
}



Submarine::Submarine(const char *path): Drawable(path), _R(1.0f), _angle(0.0f) {
    setColor(0xFF030303);
    //loadTextures("resources/submarine/texture.jpg");
}

void Submarine::tick()
{
    _position.x = _R * sin(_angle);
    _position.z = _R * cos(_angle);
    _yRot = -(270-(_angle*180/M_PI));
    _angle += 0.01;
}

bool Submarine::isActivated()
{
    return _isActivated;
}

void Submarine::activate()
{
    _isActivated = true;
}
