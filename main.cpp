
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



std::vector<Triangle*> faces;
unsigned int n = 1; // Anzahl der Unterteilungsstufen [NICHT ZU HOCH MACHEN SONST STIRBT DEIN PC!]
float sphereRadius = 1.0f;

void subdivideTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth) {
    if (depth <= 0) {
        std::vector<glm::vec3> vertices = { v1, v2, v3 };
        Triangle* t = new Triangle(program);
        t->init();
        t->setPositions(vertices);
        faces.push_back(t);
    }
    else {
        float scale = depth / (depth + 1.0f);
        glm::vec3 L_LEFT = v1;
        glm::vec3 L_RIGHT = v1 + scale * (v2 - v1);
        glm::vec3 L_UP = v1 + scale * (v3 - v1);
        subdivideTriangle(L_LEFT, L_RIGHT, L_UP, depth - 1);

        glm::vec3 R_LEFT = v2 + scale * (v1 - v2);
        glm::vec3 R_RIGHT = v2;
        glm::vec3 R_UP = v2 + scale * (v3 - v2);
        subdivideTriangle(R_LEFT, R_RIGHT, R_UP, depth - 1);

        glm::vec3 M_LEFT = v3 + scale * (v1 - v3);
        glm::vec3 M_RIGHT = v3 + scale * (v2 - v3);
        glm::vec3 M_UP = v3;
        subdivideTriangle(M_LEFT, M_RIGHT, M_UP, depth - 1);
    }
}

void approximateSphere() {
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle({ 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, n);

    for (int i = 0; i < faces.size(); i++) {
        glm::vec3 points[3];
        glBindBuffer(GL_ARRAY_BUFFER, faces[i]->positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        glm::vec3 p1 = glm::normalize(points[0]);
        glm::vec3 p2 = glm::normalize(points[1]);
        glm::vec3 p3 = glm::normalize(points[2]);

        faces[i]->setPositions({ p1, p2, p3 });
    }
}

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
    approximateSphere();

    return true;
}
/*
 Rendering.
 */

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (Triangle* t : faces) {

        t->rotate(1.0f / 60.0f, { 0.0f, 1.0f, 0.0f });
        t->render(projection, view);

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
