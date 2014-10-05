#ifndef WATER_H
#define WATER_H

int WaterInit(int, char**);

namespace Water
{
    float z(const float x, const float y, const float t);

    int	loadTexture (const char * filename,
                      unsigned char * dest,
                      const int format,
                      const unsigned int size);

    void DisplayFunc ();
};

#endif
