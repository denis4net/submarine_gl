#include "scene.h"
#include "utils.h"
#include "drawable.h"
#include "water.h"

#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

static std::vector<Drawable*> _drawables;
static std::mutex _objectOperationsMutex;
static Submarine* _submarine;
static Drawable* _sphere;

static time_t  _launchTime;

static MousePosition _lastPosition {0, 0, 0};


static struct
{
    AngleRad x, y, z;
} _rotAngles = {0, 0, 0};

void Scene::init()
{
    glClearColor(0xae/255.0, 0xf6/255.0, 0xff/255.0, 0);

    ::WaterInit(0, NULL);

    _submarine = new Submarine("resources/submarine.obj");
    _drawables.push_back(_submarine);

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    Engine::addKeyHandler({' ', launchRocket});
    Engine::addKeyHandler({'w', up});
    Engine::addKeyHandler({'s', down});

    glutMotionFunc(mouseMove);

    //run coord update thread
    new std::thread(ticker);
}


void Scene::drawBackground()
{
    glPushMatrix();
    //set lighting
    glPushMatrix();
        GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat light1_position[] = {2, 2, 0.0, 1.0};
        GLfloat light1_specular[] = {1, 1, 1, 1};

        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0);
    glPopMatrix();

    glTranslatef(-2, 0, -2);
    Water::DisplayFunc();
#if 0
    glColor4ub(0xFF, 0xFF, 0x31, 0xFF);
    glTranslatef(1.0, 1.0, 0.0);
    float sun_color[] = {0xFC/255., 0xFF/255., 0xD9/255., 0xFF/255.};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_color);
    glutSolidSphere(0.3f, 100, 100);
    float other_colors[] = {0, 0, 0, 0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, other_colors);
#endif

#ifdef  USE_2D
    Rect bRect = {{-1.0f, WATER_LINE_LEVEL}, 2.0f, 1.0f};
    Engine::fillRect(bRect, 0xaa0000ff);
    //draw sun
    Engine::fillCircle({0.3f, 0.3f, 0.0f}, 0.1f, 0xe4e726);
#endif
    glPopMatrix();
}


void Scene::drawScene()
{  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();

    glTranslatef(0, 0, -1.5f);
    glRotatef(_rotAngles.x, 1.0f, 0.0f, 0.0f);
    glRotatef(_rotAngles.y, 0.0f, 1.0f, 0.0f);
    glRotatef(_rotAngles.z, 0.0f, 0.0f, 1.0f);

    drawObjects();
    drawBackground();

    glutSwapBuffers();
}

void Scene::ticker()
{
    for (;;)
    {
        for (auto it = _drawables.begin(); it != _drawables.end(); it++)
        {
            Drawable * dObject = *it;
            dObject->tick();
            if (dObject->isShouldBeDestroyed())
            {
                _objectOperationsMutex.lock();
                _drawables.erase(it);
                *it = nullptr;
                LOG("Object %p deleted", dObject);
                delete dObject;

                _objectOperationsMutex.unlock();
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        glutPostRedisplay();
    }
}


void Scene::launchRocket()
{
    // launch rocket once in a second
    if (_launchTime != time(NULL))
        _launchTime = time(NULL);
    else
        return;

    if (_submarine->_position.y >= WATER_LINE_LEVEL)
    {
        Rocket *r = new Rocket("resources/rocket.obj");
        r->getPosition() = _submarine->getPosition();

        _objectOperationsMutex.lock();
        _drawables.push_back(r);
        _objectOperationsMutex.unlock();
    }
}
#define SUBMARINE_DEPTH_GAP 0.01f
void Scene::up()
{
    if (_submarine->getPosition().y < WATER_LINE_LEVEL)
        _submarine->getPosition().y += SUBMARINE_DEPTH_GAP;
}

void Scene::down()
{
    _submarine->getPosition().y -= SUBMARINE_DEPTH_GAP;
}


void Scene::drawObjects()
{
    _objectOperationsMutex.lock();
    for (Drawable* d: _drawables)
    {
        glPushMatrix();
        d->draw();
        glPopMatrix();
    }
    _objectOperationsMutex.unlock();
}


void Scene::mouseMove(int x, int y)
{
    MousePosition cur = {x, y, time(NULL)};
    MousePosition delta = cur - _lastPosition;
    _lastPosition = cur;

    _rotAngles.y += delta.x / ( M_PI);
    _rotAngles.x += delta.y / ( M_PI);

    //LOG("Rotate angle: %f, %f, %f", _rotAngles.x, _rotAngles.y, _rotAngles.z);
}


void Scene::drawAxis()
{
    glColor4ub(0, 0, 0, 255);
    glLineWidth(1.0);
    glBegin(GL_LINE);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}
