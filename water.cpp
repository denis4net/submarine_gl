#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include "noise.h"
#include "water.h"
#include "engine.h"


Water::Water(): Drawable({0, 0, 0})
{
    init();
    _scale=5;
    getPosition().x = 0;
    getPosition().z = 0;
}

float Water::z(const float x, const float y, const float t)
{
    const float x2 = x - 3;
    const float y2 = y + 1;
    const float xx = x2 * x2;
    const float yy = y2 * y2;
    return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) +
             Noise (10 * x, 10 * y, t, 0)) / 200);
}

int	Water::init()
{
    unsigned char total_texture[4 * 256 * 256];
    unsigned char alpha_texture[256 * 256];
    unsigned char caustic_texture[3 * 256 * 256];
    unsigned int i;

    /* Texture loading  */
    glGenTextures (1, &_texture);

    if (Water::loadTexture ("resources/alpha.jpg", alpha_texture, GL_ALPHA, 256) != 0 ||
            Water::loadTexture ("resources/reflection.jpg", caustic_texture, GL_RGB, 256) != 0)
        return 1;

    for (i = 0; i < 256 * 256; i++)
    {
        total_texture[4 * i] = caustic_texture[3 * i];
        total_texture[4 * i + 1] = caustic_texture[3 * i + 1];
        total_texture[4 * i + 2] = caustic_texture[3 * i + 2];
        total_texture[4 * i + 3] = alpha_texture[i];
    }

    glBindTexture(GL_TEXTURE_2D, _texture);
    gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, 256, 256, GL_RGBA,
                       GL_UNSIGNED_BYTE,  total_texture);
    return 0;
}

void Water::generate()
{
    const float t = glutGet (GLUT_ELAPSED_TIME) / 1000.;
    const float delta = 2. / RESOLUTION;
    _length = 2 * (RESOLUTION + 1);
    const float xn = (RESOLUTION + 1) * delta + 1;

    unsigned int i;
    unsigned int j;
    float x;
    float y;
    unsigned int indice;
    unsigned int preindice;

    /* Yes, I know, this is quite ugly... */
    float v1x;
    float v1y;
    float v1z;

    float v2x;
    float v2y;
    float v2z;

    float v3x;
    float v3y;
    float v3z;

    float vax;
    float vay;
    float vaz;

    float vbx;
    float vby;
    float vbz;

    float nx;
    float ny;
    float nz;

    float l;


    /* Vertices */
    for (j = 0; j < RESOLUTION; j++)
    {
        y = (j + 1) * delta - 1;
        for (i = 0; i <= RESOLUTION; i++)
        {
            indice = 6 * (i + j * (RESOLUTION + 1));

            x = i * delta - 1;
            _surface[indice + 3] = x;
            _surface[indice + 4] = z (x, y, t);
            _surface[indice + 5] = y;
            if (j != 0)
            {
                /* Values were computed during the previous loop */
                preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
                _surface[indice] = _surface[preindice + 3];
                _surface[indice + 1] = _surface[preindice + 4];
                _surface[indice + 2] = _surface[preindice + 5];
            }
            else
            {
                _surface[indice] = x;
                _surface[indice + 1] = z (x, -1, t);
                _surface[indice + 2] = -1;
            }
        }
    }

    /* Normals */
    for (j = 0; j < RESOLUTION; j++)
        for (i = 0; i <= RESOLUTION; i++)
        {
            indice = 6 * (i + j * (RESOLUTION + 1));

            v1x = _surface[indice + 3];
            v1y = _surface[indice + 4];
            v1z = _surface[indice + 5];

            v2x = v1x;
            v2y = _surface[indice + 1];
            v2z = _surface[indice + 2];

            if (i < RESOLUTION)
            {
                v3x = _surface[indice + 9];
                v3y = _surface[indice + 10];
                v3z = v1z;
            }
            else
            {
                v3x = xn;
                v3y = z (xn, v1z, t);
                v3z = v1z;
            }

            vax =  v2x - v1x;
            vay =  v2y - v1y;
            vaz =  v2z - v1z;

            vbx = v3x - v1x;
            vby = v3y - v1y;
            vbz = v3z - v1z;

            nx = (vby * vaz) - (vbz * vay);
            ny = (vbz * vax) - (vbx * vaz);
            nz = (vbx * vay) - (vby * vax);

            l = sqrtf (nx * nx + ny * ny + nz * nz);
            if (l != 0)
            {
                l = 1 / l;
                _normal[indice + 3] = nx * l;
                _normal[indice + 4] = ny * l;
                _normal[indice + 5] = nz * l;
            }
            else
            {
                _normal[indice + 3] = 0;
                _normal[indice + 4] = 1;
                _normal[indice + 5] = 0;
            }


            if (j != 0)
            {
                /* Values were computed during the previous loop */
                preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
                _normal[indice] = _normal[preindice + 3];
                _normal[indice + 1] = _normal[preindice + 4];
                _normal[indice + 2] = _normal[preindice + 5];
            }
            else
            {
                /* 	    v1x = v1x; */
                v1y = z (v1x, (j - 1) * delta - 1, t);
                v1z = (j - 1) * delta - 1;

                /* 	    v3x = v3x; */
                v3y = z (v3x, v2z, t);
                v3z = v2z;

                vax = v1x - v2x;
                vay = v1y - v2y;
                vaz = v1z - v2z;

                vbx = v3x - v2x;
                vby = v3y - v2y;
                vbz = v3z - v2z;

                nx = (vby * vaz) - (vbz * vay);
                ny = (vbz * vax) - (vbx * vaz);
                nz = (vbx * vay) - (vby * vax);

                l = sqrtf (nx * nx + ny * ny + nz * nz);
                if (l != 0)
                {
                    l = 1 / l;
                    _normal[indice] = nx * l;
                    _normal[indice + 1] = ny * l;
                    _normal[indice + 2] = nz * l;
                }
                else
                {
                    _normal[indice] = 0;
                    _normal[indice + 1] = 1;
                    _normal[indice + 2] = 0;
                }
            }
        }
}

int	Water::loadTexture (const char * filename,  unsigned char * dest, const int format, const unsigned int size)
{
    FILE *fd;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    unsigned char * line;

    cinfo.err = jpeg_std_error (&jerr);
    jpeg_create_decompress (&cinfo);

    if (0 == (fd = fopen(filename, "rb")))
        return 1;

    jpeg_stdio_src (&cinfo, fd);
    jpeg_read_header (&cinfo, TRUE);
    if ((cinfo.image_width != size) || (cinfo.image_height != size))
        return 1;

    if (GL_RGB == format)
    {
        if (cinfo.out_color_space == JCS_GRAYSCALE)
            return 1;
    }
    else
        if (cinfo.out_color_space != JCS_GRAYSCALE)
            return 1;

    jpeg_start_decompress (&cinfo);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        line = dest +
                (GL_RGB == format ? 3 * size : size) * cinfo.output_scanline;
        jpeg_read_scanlines (&cinfo, &line, 1);
    }
    jpeg_finish_decompress (&cinfo);
    jpeg_destroy_decompress (&cinfo);
    return 0;
}

void Water::draw()
{
    //return;

    generate();

    unsigned int indice;
    float height = -0.5;
    int widht = 1;

    glPushMatrix();
    glPushAttrib(~0);

    glEnable (GL_TEXTURE_GEN_S);
    glEnable (GL_TEXTURE_GEN_T);

    glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

    glEnable (GL_DEPTH_TEST);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glScalef(_scale, _scale, _scale);
    Engine::translate(getPosition());

    /* The ground */
#if 0
    glDisable (GL_TEXTURE_2D);

    glColor3f (1, 0.9, 0.7);
    glBegin (GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f (-1 * widht, height, -1 * widht);
    glVertex3f (-1 * widht, height,  1 * widht);
    glVertex3f ( 1 * widht, height,  1 * widht);
    glVertex3f ( 1 * widht, height, -1 * widht);
    glEnd ();
#endif

    glEnable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, _texture);
    glEnable (GL_BLEND);

    GLfloat specular[] = {1, 1, 1, 1};

    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glLightf(GL_LIGHT1, GL_SHININESS, 128.0f);

    glColor4f (1, 1, 1, 1.0);

    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_VERTEX_ARRAY);
    glNormalPointer (GL_FLOAT, 0, _normal);
    glVertexPointer (3, GL_FLOAT, 0, _surface);

    for (size_t i = 0; i < RESOLUTION; i++)
        glDrawArrays (GL_TRIANGLE_STRIP, i * _length, _length);

    //draw normals

    if (_drawNormals)
    {
        glDisable(GL_TEXTURE_2D);
        Engine::setColor(0xffff000000);
        glBegin (GL_LINES);

        for (size_t j= 0; j < RESOLUTION; j++)
            for (size_t i = 0; i <= RESOLUTION; i++)
            {
                indice = 6 * (i + j * (RESOLUTION + 1));
                glVertex3fv (&(_surface[indice]));
                glVertex3f (_surface[indice] + _normal[indice] / 50,
                            _surface[indice + 1] + _normal[indice + 1] / 50,
                            _surface[indice + 2] + _normal[indice + 2] / 50);
            }
        glEnd ();
    }

    glPopAttrib();
    glPopMatrix();
}

void Water::tick()
{

}
