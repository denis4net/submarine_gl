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
static bool _waterEnabled = true;
static MousePosition _lastPosition {0, 0, 0};


static struct
{
    AngleRad x, y, z;
} _rotAngles = {0, 0, 0};


static struct
{
    SpherePoint eye = {3, 0, 0};
    Point center = {};
    Point up = {};
    Length radius;
} _camera;

void Scene::init()
{
    glClearColor(0xae/255.0, 0xf6/255.0, 0xff/255.0, 0);

    Water* water = new Water();
    _drawables.push_back(water);


    _submarine = new Submarine("resources/submarine.obj");
    _drawables.push_back(_submarine);

    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    Engine::addKeyHandler({' ', launchRocket});
    Engine::addKeyHandler({'w', up});
    Engine::addKeyHandler({'s', down});
    Engine::addKeyHandler({'-', zoomOut});
    Engine::addKeyHandler({'=', zoomIn});

    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);
    //run coord update thread
    new std::thread(ticker);
}


void Scene::drawBackground()
{
    glPushMatrix();
        glPushMatrix();
            GLfloat light1_diffuse[] = {0.9f, 0.9f, 0.8f, 1.0};
            GLfloat light1_position[] = {0, 0, 0, 0.0};
            GLfloat disabled[] = {0, 0, 0, 0};

            glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
            glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
            glLightfv(GL_LIGHT1, GL_AMBIENT, disabled);
        glPopMatrix();
    glPopMatrix();
}


void Scene::drawScene()
{  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();

    Point eyePoint = _camera.eye.toCartesianPoint();
    gluLookAt(eyePoint.x, eyePoint.y, eyePoint.z,
              0, 0, 0,
              0, 1, 0);

    glColor3f(0, 0, 0);
    glutSolidCube(0.5f);

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

    if (_submarine->_position.y >= DEFAULT_WATER_LEVEL)
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
    if (_submarine->getPosition().y < DEFAULT_WATER_LEVEL)
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

    _camera.eye.thetta += delta.x  / 320.0;
    _camera.eye.phi += delta.y / 320.0;

    if (_camera.eye.thetta > M_PI)
        _camera.eye.thetta = 0;

    if (_camera.eye.phi > 2*M_PI)
        _camera.eye.phi = 0;
}


void Scene::drawAxis()
{
}


void Scene::water()
{
    _waterEnabled = !_waterEnabled;
}


void Scene::zoomOut()
{
    _camera.eye.r -= 0.1f;
}


void Scene::zoomIn()
{
    _camera.eye.r += 0.1f;
}


void Scene::mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        _lastPosition = {x, y};
    }
}
