#pragma once

#include <cstdlib>

typedef float Length;
typedef float AngleRad;

struct Point
{
    float x, y, z;
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
