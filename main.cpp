
#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"

#include "Triangle.h"

// Standard window width
const int WINDOW_WIDTH  = 480;
// Standard window height
const int WINDOW_HEIGHT = 480;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar  = 100.0f;


/*
Triangle triangle1(program);
Triangle triangle2(program);
Triangle triangle3(program);
Triangle triangle4(program);
Triangle triangle5(program);
Triangle triangle6(program);
Triangle triangle7(program);
Triangle triangle8(program);
*/
std::vector<Triangle*> dreiecke;

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
    // OpenGL: Set "background" color and enable depth testing.
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Construct view matrix.
    glm::vec3 eye(0.0f, 0.0f, 4.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(eye, center, up);

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

    std::vector<glm::vec3> startwerte = {
        {0.0f, 1.0f, 0.0f},  { 0.0f, 0.0f, 1.0f},   {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},  { 0.0f, 0.0f, -1.0f },  {0.0f, -1.0f, 0.0f} };

    std::vector<glm::vec3> indices = {
         {0, 4 ,1},   {0, 1, 3},    {0, 3, 2} ,  {0, 2, 4},
         {5, 1 ,4},   {5, 3, 1},    {5, 2, 3},  {5, 4, 2} };

    const std::vector<glm::vec3> colors = {
        { 1.0f, 0.0f, 0.0f}, { 1.0f, 1.0, 0.0f }, { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f } ,{ 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } };

    
    for (int i = 0; i < indices.size(); i++) {
        Triangle* kugel = new Triangle(program);
        kugel->init();
        kugel->setPositions({ startwerte[indices[i][0]], startwerte[indices[i][1]], startwerte[indices[i][2]] });
        kugel->setIndices({ 0, 1, 2 });
        kugel->setColors({ colors[indices[i][0]], colors[indices[i][1]], colors[indices[i][2]] });
        dreiecke.push_back(kugel);
    }
    
    /*
    triangle1.init();
    triangle2.init();
    triangle3.init();
    triangle4.init();
    triangle5.init();
    triangle6.init();
    triangle7.init();
    triangle8.init();

    triangle1.setPositions({ startwerte[indices[0][0]], startwerte[indices[0][1]], startwerte[indices[0][2]] });
    triangle1.setIndices({ 0, 1, 2 });
    triangle1.setColors({ colors[indices[0][0]], colors[indices[0][1]], colors[indices[0][2]] });

    triangle2.setPositions({ startwerte[indices[1][0]], startwerte[indices[1][1]], startwerte[indices[1][2]] });
    triangle2.setIndices({ 0, 1, 2 });
    triangle2.setColors({ colors[indices[1][0]], colors[indices[1][1]], colors[indices[1][2]] });
    
    triangle3.setPositions({ startwerte[indices[2][0]], startwerte[indices[2][1]], startwerte[indices[2][2]] });
    triangle3.setIndices({ 0, 1, 2 });
    triangle3.setColors({ colors[indices[2][0]], colors[indices[2][1]], colors[indices[2][2]] });

    triangle4.setPositions({ startwerte[indices[3][0]], startwerte[indices[3][1]], startwerte[indices[3][2]] });
    triangle4.setIndices({ 0, 1, 2 });
    triangle4.setColors({ colors[indices[3][0]], colors[indices[3][1]], colors[indices[3][2]] });

    triangle5.setPositions({ startwerte[indices[4][0]], startwerte[indices[4][1]], startwerte[indices[4][2]] });
    triangle5.setIndices({ 0, 1, 2 });
    triangle5.setColors({ colors[indices[4][0]], colors[indices[4][1]], colors[indices[4][2]] });

    triangle6.setPositions({ startwerte[indices[5][0]], startwerte[indices[5][1]], startwerte[indices[5][2]] });
    triangle6.setIndices({ 0, 1, 2 });
    triangle6.setColors({ colors[indices[5][0]], colors[indices[5][1]], colors[indices[5][2]] });

    triangle7.setPositions({ startwerte[indices[6][0]], startwerte[indices[6][1]], startwerte[indices[6][2]] });
    triangle7.setIndices({ 0, 1, 2 });
    triangle7.setColors({ colors[indices[6][0]], colors[indices[6][1]], colors[indices[6][2]] });

    triangle8.setPositions({ startwerte[indices[7][0]], startwerte[indices[7][1]], startwerte[indices[7][2]] });
    triangle8.setIndices({ 0, 1, 2 });
    triangle8.setColors({ colors[indices[7][0]], colors[indices[7][1]], colors[indices[7][2]] });
    */
    return true;
}

/*
 Rendering.
 */

float aaaaa = 0.0f;
void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < dreiecke.size(); i++) {
        dreiecke[i]->render(projection, view);
        dreiecke[i]->model = glm::rotate(dreiecke[i]->model, glm::radians(aaaaa), glm::vec3(0.0f, 1.0f, 1.0f));

    }
    aaaaa += 0.001f;
    if (aaaaa >= 360.0f) {
        aaaaa = 0.0f;
    }
    /*
    triangle1.render(projection, view);
    triangle2.render(projection, view);
    triangle3.render(projection, view);
    triangle4.render(projection, view);
    triangle5.render(projection, view);
    triangle6.render(projection, view);
    triangle7.render(projection, view);
    triangle8.render(projection, view);*/
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
void glutKeyboard (unsigned char keycode, int x, int y)
{
    switch (keycode) {
    case 27: // ESC
        glutDestroyWindow ( glutID );
        return;
    
    case '+':
        // do something
        break;
    case '-':
        // do something
        break;
    case 'x':
        // do something
        break;
    case 'y':
        // do something
        break;
    case 'z':
        // do something
        break;
    }
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
