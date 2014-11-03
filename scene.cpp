#include "scene.h"
#include "utils.h"
#include "drawable.h"
#include "water.h"
#include "coordinateaxis.h"
#include "background.h"
#include "ocean.h"
#include "rock.h"

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

const static GLfloat atmosphereFogColor[] = {0.7, 0.7, 0.7};
const static GLfloat backgroundColor[] = {0x89/255.0, 0xA4/255.0, 0xBE/255.0, 0xff};

void gltGetPlainEquation(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, glm::vec4& plainEquation)
{

}

/*
void glMakeShadowMatrix(glm::vec3 vPoints[3], glm::vec4 vLightPos, glm::mat4x4 destMat)
{
   glm::vec4 vPlaneEquation;
   GLfloat dot;

   gltGetPlainEquation(vPoints[0], vPoints[1], vPoints[2], vPlaneEquation);

   //scalar product
   dot = vPlaneEquation*vLightPos;

}
*/
static struct
{
    SpherePoint eye = {3, 1, 1};
    Point center = {0, 0, 0};
    Point up = {0, 1, 0};
} _camera;

void Scene::init()
{
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);

    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);

    //glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
    //glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glEnable(GL_FOG);

    glEnable(GL_POLYGON_SMOOTH);

    glDisable(GL_TEXTURE_2D);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glDisable(GL_BLEND);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glEnable(GL_COLOR_SUM);

    glEnable(GL_NORMALIZE);
    glEnable(GL_RESCALE_NORMAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // create drawable\renderable objects and add they to container

    _submarine = new Submarine();
    _drawables.push_back(_submarine);
    //_drawables.push_back(new SubmarineShadow(_submarine));

    Background *background = new Background();
    _drawables.push_back(background);

    //CoordinateAxis* axis = new CoordinateAxis();
    //_drawables.push_back(axis);

    Rock *rock = new Rock();
    _drawables.push_back(rock);

    Water* water = new Water();
    _drawables.push_back(water);

    //register keyboard handlers
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


void Scene::setLighting()
{
    glPushMatrix();
    glTranslatef(0, 1, 0);

    GLfloat light_color[] = {1.0f, 1.0f, 0.7f, 1.0};
    GLfloat light_position[] = {0, 0, 0, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};

    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glPopMatrix();
}


void Scene::drawScene()
{
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();

    Point eyePoint = _camera.eye.toCartesianPoint();
    gluLookAt(eyePoint.x, eyePoint.y, eyePoint.z,
              _camera.center.x, _camera.center.y, _camera.center.z,
              _camera.up.x, _camera.up.y, _camera.up.z);

    fog();
    drawObjects();
    setLighting();

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


void Scene::fog()
{
    bool underWater = _camera.eye.toCartesianPoint().y < WATER_FOG_LEVEL;

    if (underWater)
    {
        glFogfv(GL_FOG_COLOR, backgroundColor);
        glFogf(GL_FOG_START, 1.0f);
        glFogf(GL_FOG_END, 4.0f);
        glFogi(GL_FOG_MODE, GL_LINEAR);
    }
    else
    {
        glFogfv(GL_FOG_COLOR, atmosphereFogColor);
        glFogf(GL_FOG_START, 3.5f);
        glFogf(GL_FOG_END, 7.0f);
        glFogi(GL_FOG_MODE, GL_LINEAR);
    }
}


void Scene::drawShadowModel(Drawable *a)
{
}
