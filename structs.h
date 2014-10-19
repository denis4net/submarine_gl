#pragma once

#include <cstdlib>
#include <vector>
#include <glm/glm.hpp>

typedef float Length;
typedef float AngleRad;

typedef glm::vec3 Vector3D;
typedef glm::vec2 Vector2D;
typedef glm::vec3 Vertex;

typedef std::vector<Vector3D> VerticesArray;
typedef std::vector<VerticesArray> ObjectArray;
typedef std::vector<Vector2D> TexturesCoordArray;
typedef std::vector<Vector3D> NormalsArary;
typedef unsigned int Index;
typedef std::vector<Index> IndicesArray;
typedef unsigned int Color;

struct MousePosition
{
    int x;
    int y;
    time_t ts;

    MousePosition operator-(MousePosition& o)
    {
        return {x-o.x, y-o.y, ts-o.ts};
    }
};

struct Point
{
    float x, y, z;
};

struct SpherePoint
{
    Length r;
    AngleRad thetta, phi;

    struct Point toCartesianPoint()
    {
        return { r*sin(thetta)*cos(phi),
                 r*sin(thetta)*sin(phi),
                 r*cos(thetta) };
    }
};

struct Line
{
    Point a, b;
};

struct Circle
{
    Point r;
    Length l;
};

struct Rect
{
    Point start;
    Length width;
    Length height;
};

struct KeyHandler
{
    int keycode;
    void (*fun)();
};
