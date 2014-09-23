#include "scene.h"
#include "utils.h"
#include "drawable.h"
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

static std::vector<Drawable*> _drawables;
static std::mutex _objectOperationsMutex;
static Submarine* _submarine;
static time_t  _launchTime;


void Scene::init()
{
    glClearColor(0xae/255.0, 0xf6/255.0, 0xff/255.0, 0);
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    _submarine = new Submarine({0.0f, -0.2f, 0.0f});
    _drawables.push_back(_submarine);

    Engine::addKeyHandler({' ', launchRocket});
    Engine::addKeyHandler({'w', up});
    Engine::addKeyHandler({'s', down});

    //run coord update thread
    new std::thread(ticker);
}


void Scene::drawBackground()
{
    Rect bRect = {{-1.0f, WATER_LINE_LEVEL}, 2.0f, 1.0f};
    Engine::fillRect(bRect, 0x000000ff);

    //draw sun
    Engine::fillCircle({0.3f, 0.3f, 0.0f}, 0.1f, 0xe4e726);
}


void Scene::drawScene()
{  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();                                       // Reset The View
    glTranslatef(0, 0, -1.2);

    drawObjects();
    drawBackground();
    glutSwapBuffers();
    glutPostRedisplay();
}

void Scene::ticker()
{
    for (;;)
    {
        for (auto it = _drawables.begin(); it != _drawables.end(); it++)
        {
            Drawable * dObject = *it;
            if (dObject == nullptr)
                continue;

            dObject->tick();
            if (dObject->isShouldBeDestroyed())
            {
                _objectOperationsMutex.lock();
                //_drawables.erase(it);
                *it = nullptr;
                //delete dObject;
                _objectOperationsMutex.unlock();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}


void Scene::launchRocket()
{
    if (_launchTime != time(NULL))
        _launchTime = time(NULL);
    else
        return;

    if (_submarine->position.y >= WATER_LINE_LEVEL)
    {
        Rocket *r = new Rocket(_drawables.front()->getPosition());
        _objectOperationsMutex.lock();
        _drawables.push_back(r);
        _objectOperationsMutex.unlock();
    }
}
#define SUBMARINE_DEPTH_GAP 0.01f
void Scene::up()
{
    if (_submarine->position.y < WATER_LINE_LEVEL)
        _submarine->position.y += SUBMARINE_DEPTH_GAP;
}

void Scene::down()
{
    _submarine->position.y -= SUBMARINE_DEPTH_GAP;
}


void Scene::drawObjects()
{
    _objectOperationsMutex.lock();
    for (Drawable* d: _drawables)
    {
        d->draw();
    }
    _objectOperationsMutex.unlock();
}
