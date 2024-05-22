
#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#include "GLSLProgram.h"
#include "GLTools.h"

#include "Triangle.h"
#include "Line.h"
#include "Sphere.h"

// Standard window width
const int WINDOW_WIDTH = 640;
// Standard window height
const int WINDOW_HEIGHT = 640;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar = 100.0f;

glm::vec3 eyePoint(0.0f, 0.0f, 5.0f);
glm::vec3 centerPoint = eyePoint - glm::normalize(eyePoint);
glm::vec3 up(0.0f, 1.0f, 0.0f);
float mouseSpeed = 0.01f;

Sphere sun(program);
Sphere planet(program);
Sphere moon(program);
Sphere moon_moon(program);

float sunSpeed = 0.0f;
float planetSpeed = 0.0f;
float moonSpeed = 0.0f;
float moon_moonSpeed = 0.0f;

bool shouldRoate = GL_TRUE;
/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
    // OpenGL: Set "background" color and enable depth testing.
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    view = glm::lookAt(eyePoint, centerPoint, up);

    // Create a shader program and set light direction.
    if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
        std::cerr << program.log();
        return false;
    }

    if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
        std::cerr << program.log();
        return false;
    }

    if (!program.link()) {
        std::cerr << program.log();
        return false;
    }

    sun.init(0.5, { 1.0f,1.0f,0.0f });
    sun.setPosition({ 0.0f,0.0f,0.0f });

    planet.init(0.2f, { 0.0f,1.0f,0.0f });
    planet.setPosition({ 1.5f,0.0f,0.0f });
    planet.rotateAround(-45.0f, { 0.0f,0.0f,1.0f }, planet.getSphereCenter());
    planet.setRotationAxis({ 1.0f,1.0f,0.0f });

    moon.init(0.1f, { 0.1f,0.9f,1.0f });
    moon.setPosition({ 1.9f, 0.0f, 0.0f });
    moon.rotateAround(-45.0f, { 0.0f,0.0f,1.0f }, planet.getSphereCenter());
    moon.setRotationAxis({ 1.0f,1.0f,0.0f });

    moon_moon.init(0.05f, { 1.0f,0.0f,0.78f });
    moon_moon.setPosition({ 2.3f, 0.0f, 0.0f });
    moon_moon.rotateAround(-45.0f, { 0.0f,0.0f,1.0f }, planet.getSphereCenter());
    moon_moon.setRotationAxis({ 1.0f,1.0f,0.0f });

    sun.setChild(&planet);
    planet.setChild(&moon);
    moon.setChild(&moon_moon);

    return true;
}
/*
 Rendering.
 */

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sun.render(projection, view);
    planet.render(projection, view);
    moon.render(projection, view);
    moon_moon.render(projection, view);

    if (shouldRoate) {
        sun.rotateAround(sunSpeed, { 0.0f, 1.0f, 0.0f }, sun.getSphereCenter());
        planet.rotateAround(planetSpeed, sun.getRotationAxis(), sun.getSphereCenter());
        moon.rotateAround(moonSpeed, planet.getRotationAxis(), planet.getSphereCenter());
        moon_moon.rotateAround(moon_moonSpeed, moon.getRotationAxis(), moon.getSphereCenter());
    }
}

void glutDisplay()
{
    render();
    glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize(int width, int height)
{
    // Division by zero is bad...
    height = height < 1 ? 1 : height;
    glViewport(0, 0, width, height);

    // Construct projection matrix.
    projection = glm::perspective(45.0f, (float)width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard(unsigned char keycode, int x, int y)
{
    glm::vec3 cameraDirection;
    glm::vec3 cameraHorizontal;
    glm::vec3 cameraUp;

    float speedChange = 0.02f;
    float speedLimit = 0.2f;

    switch (keycode) {
    case 27: // ESC
        glutDestroyWindow(glutID);
        return;
    case '+':
        eyePoint -= (eyePoint - centerPoint) * 0.1f;
        centerPoint = eyePoint - glm::normalize(eyePoint - centerPoint);
        break;
    case '-':
        eyePoint += (eyePoint - centerPoint) * 0.1f;
        centerPoint = eyePoint - glm::normalize(eyePoint - centerPoint);
        break;
    case 'g':   //Rotation anhalten
        shouldRoate = !shouldRoate;
        break;
    case 'f':   //Schneller
        sunSpeed = sunSpeed < speedLimit - speedChange ? sunSpeed + speedChange : speedLimit;
        planetSpeed = planetSpeed < speedLimit - speedChange ? planetSpeed + speedChange : speedLimit;
        moonSpeed = moonSpeed < speedLimit - speedChange ? moonSpeed + speedChange : speedLimit;
        moon_moonSpeed = moon_moonSpeed < speedLimit - speedChange ? moon_moonSpeed + speedChange : speedLimit;
        break;
    case 'd':   //Langsamer
        sunSpeed = sunSpeed > speedChange ? sunSpeed - speedChange : 0;
        planetSpeed = planetSpeed > speedChange ? planetSpeed - speedChange : 0;
        moonSpeed = moonSpeed > speedChange ? moonSpeed - speedChange : 0;
        moon_moonSpeed = moon_moonSpeed > speedChange ? moon_moonSpeed - speedChange : 0;
        break;
    }



    view = glm::lookAt(eyePoint, centerPoint, up);
    glutPostRedisplay();
}

void glutMotion(int x, int y) {
    float w = WINDOW_WIDTH / 2.0f;
    float h = WINDOW_HEIGHT / 2.0f;
    float xMotionPercent = x - w >= 0 ? 1 / (w / x) : w / x;
    float yMotionPercent = y - h >= 0 ? 1 / (h / y) : h / y;
    glm::vec3 cameraDirection;
    glm::vec3 cameraHorizontal;
    glm::vec3 cameraUp;
    if (x < w) {
        cameraDirection = glm::normalize(eyePoint - centerPoint);
        cameraHorizontal = glm::normalize(glm::cross(up, cameraDirection));
        centerPoint += cameraHorizontal * -mouseSpeed * xMotionPercent;
    }
    else if (x > w) {
        cameraDirection = glm::normalize(eyePoint - centerPoint);
        cameraHorizontal = glm::normalize(glm::cross(up, cameraDirection));
        centerPoint += cameraHorizontal * mouseSpeed * xMotionPercent;
    }
    if (y > h) {
        cameraDirection = glm::normalize(eyePoint - centerPoint);
        cameraHorizontal = glm::normalize(glm::cross(up, cameraDirection));
        cameraUp = glm::normalize(glm::cross(cameraHorizontal, cameraDirection));
        if (cameraUp.y < -0.1f) {
            centerPoint += cameraUp * mouseSpeed * yMotionPercent;
        }
    }
    else if (y < h) {
        cameraDirection = glm::normalize(eyePoint - centerPoint);
        cameraHorizontal = glm::normalize(glm::cross(up, cameraDirection));
        cameraUp = glm::normalize(glm::cross(cameraHorizontal, cameraDirection));
        if (cameraUp.y < -0.1f) {
            centerPoint += cameraUp * -mouseSpeed * yMotionPercent;
        }
    }
    view = glm::lookAt(eyePoint, centerPoint, up);
    glutWarpPointer(w, h);
    glutPostRedisplay();
}
void glutMouse(int button, int status, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && status == GLUT_DOWN) {
        glutSetCursor(GLUT_CURSOR_NONE);
    }
    else if (button == GLUT_LEFT_BUTTON && status == GLUT_UP) {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }
}


int main(int argc, char** argv)
{
    // GLUT: Initialize freeglut library (window toolkit).
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(40, 40);
    glutInit(&argc, argv);

    // GLUT: Create a window and opengl context (version 4.3 core profile).
    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("Aufgabenblatt 01");
    glutID = glutGetWindow();

    // GLEW: Load opengl extensions
    //glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }
#if _DEBUG
    if (glDebugMessageCallback) {
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(cg::glErrorVerboseCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            nullptr,
            true); // get all debug messages
    }
    else {
        std::cout << "glDebugMessageCallback not available" << std::endl;
    }
#endif

    // GLUT: Set callbacks for events.
    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc(glutDisplay); // redisplay when idle

    glutKeyboardFunc(glutKeyboard);

    glutMotionFunc(glutMotion);
    glutMouseFunc(glutMouse);

    // init vertex-array-objects.
    bool result = init();
    if (!result) {
        return -2;
    }
    // GLUT: Loop until the user closes the window
    // rendering & event handling
    glutMainLoop();

    // Cleanup in destructors:
    // Objects will be released in ~Object
    // Shader program will be released in ~GLSLProgram

    return 0;
}
