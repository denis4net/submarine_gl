#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "structs.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define DEFAULT_DRAWABLE_COLOR 0xFF000000

#define COLOR_BACKGROUND 0xAEF6FF
#define COLOR_SUBMARINE 0x999999
#define COLOR_ROCKET 0xFF222222
#define SUBMARINE_GAP 0.01f


#define ROCKET_Y_SPEED 0.01f
#define ROCKET_X_SPEED 0.003f


class Drawable
{
protected:
    bool _shoudBeDestroyed = false;
    const aiScene* _scene = nullptr;

    unsigned int _color = DEFAULT_DRAWABLE_COLOR;
    AngleRad _xRot, _yRot, _zRot;
    float _scale = 0.5f;

    float _materialSpecular[4] = {1.0, 1.0, 1.0, 1.0};

    bool loadFromFile(const char* path);
    bool loadTextures(const char* filename);

    Assimp::Importer _importer;
    std::vector<int> _textures;
public:
    Drawable(Point p = {0.0f, 0.0f, 0.0f}): _position(p) {};
    Drawable() {}
    Drawable(const char* path) {

        loadFromFile(path);

        _xRot = 0;
        _yRot = 0;
        _zRot = 0;
    }

    Point _position;


    void setColor(unsigned int color);
    virtual void draw();
    virtual void tick() = 0;
    virtual bool isShouldBeDestroyed();
    virtual Point& getPosition();
};

class Rocket : public Drawable
{
private:
    AngleRad direction;
    unsigned int ticks;
public:
    Rocket(const char* path);
    virtual void tick() override;
};

class Submarine: public Drawable
{
    bool _isActivated;
    float _R;
    AngleRad _angle;
public:
    Submarine(const char* path);
    virtual void tick() override;

    virtual bool isActivated();
    virtual void activate();
};

#endif // DRAWABLE_H
