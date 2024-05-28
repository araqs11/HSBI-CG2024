
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
const int WINDOW_WIDTH  = 640;
// Standard window height
const int WINDOW_HEIGHT = 640;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar  = 100.0f;

glm::vec3 eyePoint(0.0f, 0.0f, 5.0f);
glm::vec3 centerPoint = eyePoint - glm::normalize(eyePoint);
glm::vec3 up(0.0f, 1.0f, 0.0f);
float mouseSpeed = 0.01f;

unsigned int n = 2;
Sphere sonne(program);
Sphere erde(program);
Sphere mond(program);
Sphere mondmond(program);
bool darfsichdrehen = true;
float rotationSpeed = 0.2f;

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
    sonne.init(0.4f, 0, 1);

    erde.init(0.3f, 0, 2);
    erde.setPosition({ 2.0f,0.0f,0.0f });
    erde.rotate(-45.0f,glm::vec3(0.0f,0.0f,1.0f),erde.center);

    mond.init(0.1f, 0, 3);
    mond.setPosition({ 1.5f,0.5f,0.0f });
    mond.rotate(-45.0f, glm::vec3(0.0f, 0.0f, 1.0f), mond.center);

    mondmond.init(0.05f, 0, 4);
    mondmond.setPosition({ 1.4f,0.6f,0.0f });
    mondmond.rotate(-45.0f, glm::vec3(0.0f, 0.0f, 1.0f), mondmond.center);
    return true;

}
/*
 Rendering.
 */

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    sonne.render(projection, view);
    erde.render(projection, view);
    mond.render(projection, view);
    mondmond.render(projection, view);


    if (darfsichdrehen) {
        sonne.rotate(1.0f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), sonne.center);

        erde.rotate(0.6f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), sonne.center);
        erde.rotate(0.6f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), erde.center);

        mond.rotate(0.6f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), sonne.center);
        mond.rotate(0.6f * rotationSpeed, glm::vec3(1.0f, 0.0f, 0.0f), erde.center);
        mond.rotate(0.4f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), mond.center);

        mondmond.rotate(0.6f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), sonne.center);
        mondmond.rotate(0.6f * rotationSpeed, glm::vec3(1.0f, 0.0f, 0.0f), erde.center);
        mondmond.rotate(0.4f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), mond.center);
        mondmond.rotate(0.4f * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f), mondmond.center);
    }
    

}

void glutDisplay ()
{
    render();
    glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize (int width, int height)
{
    // Division by zero is bad...
    height = height < 1 ? 1 : height;
    glViewport(0, 0, width, height);
  
    // Construct projection matrix.
    projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard(unsigned char keycode, int x, int y)
{
    glm::vec3 cameraDirection;
    glm::vec3 cameraHorizontal;
    glm::vec3 cameraUp;

    glm::vec3 xAxisPoints[2];
    glm::vec3 yAxisPoints[2];
    glm::vec3 zAxisPoints[2];
    switch (keycode) {
    case 27: // ESC
        glutDestroyWindow(glutID);
        return;
    case 'g':
        darfsichdrehen = !darfsichdrehen;
        break;
    case 'd':
        if (rotationSpeed < 2.0f) {
            rotationSpeed += 0.1f;
        }        break;
    case 'f':
        if (rotationSpeed > 0.2f) {
            rotationSpeed -= 0.1f;
        }
        break;
    }
    // sphere.init aufrufen mit Parameter N und die Sphere.render mit view und projetction

       
    view = glm::lookAt(eyePoint, centerPoint, up);
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    // GLUT: Initialize freeglut library (window toolkit).
    glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);
  
    // GLUT: Create a window and opengl context (version 4.3 core profile).
    glutInitContextVersion(4, 3);
    glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  
    glutCreateWindow("Aufgabenblatt 03");
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
    } else {
        std::cout << "glDebugMessageCallback not available" << std::endl;
    }
#endif

    // GLUT: Set callbacks for events.
    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc   (glutDisplay); // redisplay when idle

    glutKeyboardFunc(glutKeyboard); 
  
    // init vertex-array-objects.
    bool result = init();
    if (!result) {
        return -2;
    }
        
    // GLUT: Loop until the user closes the window
    // rendering & event handling
    glutMainLoop ();
  
    // Cleanup in destructors:
    // Objects will be released in ~Object
    // Shader program will be released in ~GLSLProgram
  
    return 0;
}
