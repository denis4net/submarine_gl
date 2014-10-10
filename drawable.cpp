#include "drawable.h"
#include "engine.h"
#include "utils.h"
#include "structs.h"

#include <GL/gl.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

using namespace std;


bool Drawable::loadFromFile(const char *path)
{
#if USE_MYSELF_LOADER
    FILE * file = fopen(path, "r");
    if (file == nullptr)
    {
        printf("Impossible to open the file !\n");
        return false;
    }

    VerticesArray temp_vertices;
    TexturesCoordArray temp_uvs;
    NormalsArary temp_normals;

    IndicesArray vertexIndices;
    IndicesArray uvIndices;
    IndicesArray normalIndices;
    size_t lastIndex = 0;

    while (true)
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);

        }
        else if ( strcmp( lineHeader, "f" ) == 0 ){

            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                    &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                    &vertexIndex[2], &uvIndex[2], &normalIndex[2] );

            if (matches != 9)
            {
                ERROR("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]-1);
            vertexIndices.push_back(vertexIndex[1]-1);
            vertexIndices.push_back(vertexIndex[2]-1);
            uvIndices    .push_back(uvIndex[0]-1);
            uvIndices    .push_back(uvIndex[1]-1);
            uvIndices    .push_back(uvIndex[2]-1);
            normalIndices.push_back(normalIndex[0]-1);
            normalIndices.push_back(normalIndex[1]-1);
            normalIndices.push_back(normalIndex[2]-1);
        }
        else if (strcmp(lineHeader, "o") == 0)
        {
        }
    }

    VerticesArray vertices;

    for (; lastIndex < vertexIndices.size(); lastIndex++)
    {
        Index vi = vertexIndices[lastIndex];
        Index uvi = uvIndices[lastIndex];
        Index ni = normalIndices[lastIndex];

        vertices.push_back(temp_vertices[vi]);
        _normals.push_back(temp_normals[ni]);
        if (uvi != (Index)(-1))
            _uvs.push_back(temp_uvs[uvi]);
    }

    _objects.push_back(vertices);

    LOG("Loaded %ul vertices from \"%s\", object count: %ul", temp_vertices.size(), path, _objects.size());

    fclose(file);
#endif
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



void Drawable::setColor(unsigned int color)
{
    _color = color;
}

void Drawable::draw()
{
    glPushMatrix();

    glColor4ub(0xff & (_color >> 16), 0xff & (_color >> 8), 0xff & _color, 0xff & (_color >> 24));

    glTranslatef(_position.x, _position.y, _position.z);
    glRotatef(_xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(_yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(_zRot, 0.0f, 0.0f, 1.0f);

    glScalef(_scale, _scale, _scale);
#if USE_MYSELF_LOADER
    for (VerticesArray& object: _objects)
    {
        glBegin(GL_TRIANGLES);
        for (Vector3D& v: object)
        {
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
#endif
    for (size_t meshIndex = 0; meshIndex < _scene->mNumMeshes; meshIndex++)
    {
        aiMesh *mesh = _scene->mMeshes[meshIndex];

        for (size_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex ++)
        {
            aiFace face = mesh->mFaces[faceIndex];

            glBegin(GL_POLYGON);
            for (size_t i = 0; i < face.mNumIndices; i++)
            {
                size_t vertexIndex = face.mIndices[i];
                glVertex3f(mesh->mVertices[vertexIndex].x,
                           mesh->mVertices[vertexIndex].y,
                           mesh->mVertices[vertexIndex].z);

                    glNormal3f( mesh->mNormals[vertexIndex].x,
                                mesh->mNormals[vertexIndex].y,
                                mesh->mNormals[vertexIndex].z);
            }
            glEnd();
        }
    }
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
    _scale = 0.01f;
}

void Rocket::draw()
{
#if defined(USE_2D)
    Engine::fillRect({{_position.x, _position.y, _position.z}, 0.03f, 0.06f}, COLOR_ROCKET);
    Point rocketHeadPos = _position;
    rocketHeadPos.x += 0.015f;
    Engine::fillCircle(rocketHeadPos, 0.015f, COLOR_ROCKET);
#endif
    Drawable::draw();
}

void Rocket::tick()
{
    ticks += 1;

    _position.y += ROCKET_Y_SPEED;
    _position.x -= ROCKET_X_SPEED;
}



#ifdef USE_2D
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
#else
void Submarine::draw()
{
    Drawable::draw();
}

#endif

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
