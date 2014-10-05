#include "drawable.h"
#include "engine.h"
#include "utils.h"
#include "structs.h"

#include <GL/gl.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>


bool Drawable::loadFromFile(const char *path)
{
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
            std::string vertex1, vertex2, vertex3;

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
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    _vertices.clear();

    for (size_t i = 0; i < vertexIndices.size(); i++)
    {
        Index vi = vertexIndices[i];
        Index uvi = uvIndices[i];
        Index ni = normalIndices[i];

        _vertices.push_back(temp_vertices[vi]);
        _normals.push_back(temp_normals[uvi]);
        _uvs.push_back(temp_uvs[uvi]);
    }

    LOG("Loaded %ul vertices from \"%s\", mesh count: %ul", temp_vertices.size(), path, _vertices.size());

    fclose(file);
    return true;
}

Drawable::Drawable(Point p): _position(p), _shoudBeDestroyed(false)
{}

void Drawable::draw()
{
    glPushMatrix();
    glColor3ub(0, 0, 0);

    glTranslatef(_position.x, _position.y, _position.z);

    glBegin(GL_TRIANGLES);
    for (Vector3D& v: _vertices)
    {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
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


Rocket::Rocket(Point p): Drawable(p), ticks(0)
{}

void Rocket::draw()
{
    Engine::fillRect({{_position.x, _position.y, _position.z}, 0.03f, 0.06f}, COLOR_ROCKET);
    Point rocketHeadPos = _position;
    rocketHeadPos.x += 0.015f;
    Engine::fillCircle(rocketHeadPos, 0.015f, COLOR_ROCKET);
}

void Rocket::tick()
{
    ticks += 1;

    _position.y += ROCKET_Y_SPEED;
    _position.x -= ROCKET_X_SPEED;
}


Submarine::Submarine(Point p): Drawable(p), _isActivated(false)
{

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
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    Drawable::draw();
    glPopMatrix();
}

#endif

void Submarine::tick()
{
    _position.x =  _position.x - SUBMARINE_GAP > -1.0f ?
                _position.x - SUBMARINE_GAP : 1.0f;
}

bool Submarine::isActivated()
{
    return _isActivated;
}

void Submarine::activate()
{
    _isActivated = true;
}
