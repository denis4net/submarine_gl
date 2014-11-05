#include "rock.h"

Rock::Rock(): Drawable(ROCK_MODEL_FILE)
{
    setColor(0xFF000000);
    setShininess(100);
    setSpecular(0.3);
    _scale = 0.7f;
    _position.y = -1.0f;
    _position.z = -3.0f;
    _position.x = -3.0f;
}

void Rock::tick()
{

}
