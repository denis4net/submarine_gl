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

GLfloat light_color[] = {1.0f, 1.0f, 0.7f, 1.0};
GLfloat light_position[] = {0, 1.5, 0, 1};
GLfloat light_specular[] = {1, 1, 1, 1};
Point light = {0, 1.5, 0};
Point center = {0, 0, 0};


float params1[4] = { 1, 0, 0, 0 };
float params2[4] = { 0, 1, 0, 0 };
float params3[4] = { 0, 0, 1, 0 };
float params4[4] = { 0, 0, 0, 1 };

static GLuint shadowMap, diffuseMap, ambientMap;
static int width, height;

int shadowMapSize = 1024;
float mv[16];
float pr[16];

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
    //glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_FOG);

    glEnable(GL_POLYGON_SMOOTH);

    glDisable(GL_TEXTURE_2D);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

    //lLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    //glEnable(GL_COLOR_SUM);

   // glEnable(GL_NORMALIZE);
    //glEnable(GL_RESCALE_NORMAL);

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // create drawable\renderable objects and add they to container

    _submarine = new Submarine();
    _drawables.push_back(_submarine);
    //_drawables.push_back(new SubmarineShadow(_submarine));

    Background *background = new Background();
    _drawables.push_back(background);

    //CoordinateAxis* axis = new CoordinateAxis();
    //_drawables.push_back(axis);

    //Rock *rock = new Rock();
    //_drawables.push_back(rock);

    //Water* water = new Water();
    //_drawables.push_back(water);

    //register keyboard handlers
    Engine::addKeyHandler({' ', launchRocket});
    Engine::addKeyHandler({'w', up});
    Engine::addKeyHandler({'s', down});
    Engine::addKeyHandler({'-', zoomOut});
    Engine::addKeyHandler({'=', zoomIn});

    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);

    initShadow();

    //run coord update thread
    new std::thread(ticker);
}


void Scene::setLighting()
{
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
}


void Scene::reshape ( int w, int h )
{
    glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ();

    Point eye = _camera.eye.toCartesianPoint();
    gluPerspective( 60.0, (GLfloat)w/(GLfloat)h, 0.1, 10.0 );
    gluLookAt      ( eye.x, eye.y, eye.z,    		// eye
                     _camera.center.x, _camera.center.y, _camera.center.z,  // center
                     0, 1, 0 );              		// up

    glMatrixMode   ( GL_MODELVIEW );
    glLoadIdentity ();

    width  = w;
    height = h;
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


void Scene::render()
{

    renderToShadowMap ();						// compute shadow map

    // clear buffers
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    reshape ( Engine::getWidht(), Engine::getHeight() );					// setup modelview and projection

    glMatrixMode   ( GL_MODELVIEW );
    glPushMatrix   ();


    // setup shadowing
    glActiveTextureARB ( GL_TEXTURE1_ARB );
    glBindTexture      ( GL_TEXTURE_2D, shadowMap );

    glEnable ( GL_TEXTURE_2D    );
    glEnable ( GL_TEXTURE_GEN_S );
    glEnable ( GL_TEXTURE_GEN_T );
    glEnable ( GL_TEXTURE_GEN_R );
    glEnable ( GL_TEXTURE_GEN_Q );

    glTexGenfv ( GL_S, GL_EYE_PLANE, params1 );
    glTexGenfv ( GL_T, GL_EYE_PLANE, params2 );
    glTexGenfv ( GL_R, GL_EYE_PLANE, params3 );
    glTexGenfv ( GL_Q, GL_EYE_PLANE, params4 );

    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // set correct texcoord transform
    glMatrixMode  ( GL_TEXTURE );
    glPushMatrix  ();

    glLoadIdentity ();
    glTranslatef   ( 0.5, 0.5, 0.5 );     // remap from [-1,1]^2 to [0,1]^2
    glScalef       ( 0.5, 0.5, 0.5 );
    glMultMatrixf  ( pr );
    glMultMatrixf  ( mv );

    glActiveTextureARB ( GL_TEXTURE0_ARB );

    renderScene ();
    renderPlane();

    glActiveTextureARB ( GL_TEXTURE1_ARB );
    glDisable          ( GL_TEXTURE_2D   );
    glActiveTextureARB ( GL_TEXTURE0_ARB );

    // draw the light
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix  ();
    glPushMatrix ();

    glTranslatef       ( light_position[0], light_position[1], light_position[2]);
    glActiveTextureARB ( GL_TEXTURE0_ARB );
    glDisable          ( GL_TEXTURE_2D );
    glutSolidSphere    ( 0.1f, 15, 15 );
    glPopMatrix        ();

    glMatrixMode ( GL_TEXTURE );
    glPopMatrix  ();

    glMatrixMode ( GL_MODELVIEW );

    glutSwapBuffers ();
}


void Scene::initShadow() //OK
{
    glClearStencil ( 0 );
    glEnable       ( GL_DEPTH_TEST );
    glDepthFunc    ( GL_LESS );

    glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
    glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    glGenTextures   ( 1, &shadowMap );
    glBindTexture   ( GL_TEXTURE_2D, shadowMap );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexParameteri  ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
    glTexParameteri  ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );

    glTexGeni        ( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
    glTexGeni        ( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
    glTexGeni        ( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
    glTexGeni        ( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );

    glTexImage2D     ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
                       GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
}


void Scene::renderToShadowMap() //OK
{
    glColorMask ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glDisable   ( GL_TEXTURE_2D );

    glEnable        ( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset ( 4, 4 );

    // setup projection
    glViewport ( 0, 0, shadowMapSize, shadowMapSize );
    glClear    ( GL_DEPTH_BUFFER_BIT );

    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ();

    gluPerspective ( 120, 1, 0.1, 60 );
    gluLookAt      ( light.x, light.y, light.z,		// eye
                     center.x, center.x, center.z,	// center
                     0, 0, 1 );						// up

    glMatrixMode   ( GL_MODELVIEW );
    glLoadIdentity ();

    // get modelview and projections matrices
    glGetFloatv ( GL_MODELVIEW_MATRIX,  mv );
    glGetFloatv ( GL_PROJECTION_MATRIX, pr );

    // now render scene from light position
    renderCube();
    _submarine->draw();

    // copy depth map into texture
    glBindTexture    ( GL_TEXTURE_2D, shadowMap );
    glCopyTexImage2D ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, shadowMapSize, shadowMapSize, 0 );

    // restore state
    glDisable        ( GL_POLYGON_OFFSET_FILL );
    glColorMask      ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glEnable         ( GL_TEXTURE_2D );
}


void Scene::renderScene()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                                                    // draw unlit geometry
        glMatrixMode       ( GL_MODELVIEW );
        glColor3f          ( 1.0, 0.5, 0.5 );
        glActiveTextureARB ( GL_TEXTURE0_ARB );

        glMatrixMode ( GL_MODELVIEW );
        renderCube();
}


void Scene::renderCube()
{
    glPushMatrix();
        glColor3f(0, 0, 1);
        glTranslatef(0, 1, 0);
        glutSolidCube(0.3f);
    glPopMatrix();


    glPushMatrix();
        glColor3f(1, 0, 0);
        glTranslatef(0, 1, 0.4);
        glutSolidCube(0.3f);
    glPopMatrix();


    glPushMatrix();
        glColor3f(1, 1, 0);
        glTranslatef(0, 1, -0.4);
        glutSolidCube(0.3f);
    glPopMatrix();
}


void Scene::renderPlane()
{
    glPushMatrix();
        glColor3f(0, 1, 0);
        glBegin(GL_QUADS);
            glVertex3f(-3, 0, -3);
            glVertex3f(-3, 0, 3);
            glVertex3f(3, 0, 3);
            glVertex3f(3, 0, -3);
        glEnd();
    glPopMatrix();
}
