
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
unsigned int n = 4; // Anzahl der Unterteilungsstufen [NICHT ZU HOCH MACHEN SONST STIRBT DEIN PC!]
float sphereRadius = 1.0f;  // Skaliert die gr��e der Kugel

Line x_AxisLocal(program), y_AxisLocal(program), z_AxisLocal(program); // lokale x-, y- und z-Achse
Line x_AxisGlobal(program), y_AxisGlobal(program), z_AxisGlobal(program);// globale x-, y- und z-Achse
bool cs_switch = GL_FALSE; // Bestimmt welches Koordinatensystem angezeigt wird

void subdivideTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth) {
    //Sobald die Erforderte tiefe erreicht wurde werden die bis hierhin errechneten Punkte in Tats�chliche dreiecks Objekte eingeschrieben,
    // und dann im globalen vector "faces" abgespeichert
    if (depth <= 0) {
        std::vector<glm::vec3> vertices = { v1, v2, v3 };
        Triangle* t = new Triangle(program);
        t->init();
        t->setPositions(vertices);
        faces.push_back(t);
    }
    else { // Teilt ein Dreieck in 3 Dreiecke der vorherigen Tiefe auf.Dies geschieht dadurch, dass eine Ecke des originalen Dreiecks (P1),
           // mit zwei errechneten Punkten die auf den Vektoren von P1 zu P2 und P1 zu P3 liegen. Danach werden diese errechneten Dreiecke rekursiv wieder aufgeteilt.
           // hierbei ist zu beachten, dass die neu errechneten Punkte der Dreiecke noch nicht die passende Entfernung zum Mittelpunkt haben. Diese wird sp�ter ausgerechnet
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
    //Starte werte f�r Sphere mit 8 Seiten
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle({ 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, n);

    //iteriert durch die abgespeicherten dreiecke durch und fixt die Position der Punkte durch anpassung der Entfernung zum mittelpunkt
    for (int i = 0; i < faces.size(); i++) {
        glm::vec3 points[3];
        glBindBuffer(GL_ARRAY_BUFFER, faces[i]->positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        //normalisiert den Vector zu der l�nge 1 und multiplieziertdann wieder mit den gew�nschten Radius
        glm::vec3 p1 = glm::normalize(points[0]) * sphereRadius;
        glm::vec3 p2 = glm::normalize(points[1]) * sphereRadius;
        glm::vec3 p3 = glm::normalize(points[2]) * sphereRadius;

        //Speichert die neuen Positionen ab
        faces[i]->setPositions({ p1, p2, p3 });
    }
}

//Erstellt das lokale Koordinatensystem
void initLocalCS() {
    x_AxisLocal.init();
    x_AxisLocal.setPositions({ {0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f} });
    x_AxisLocal.setColors({ {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });

    y_AxisLocal.init();
    y_AxisLocal.setPositions({ {0.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f} });
    y_AxisLocal.setColors({ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });

    z_AxisLocal.init();
    z_AxisLocal.setPositions({ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.5f} });
    z_AxisLocal.setColors({ {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });
}

//Erstellt das globale Koordinatensystem
void initGlobalCS() {
    x_AxisGlobal.init();
    x_AxisGlobal.setPositions({ {0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f} });
    x_AxisGlobal.setColors({ {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });

    y_AxisGlobal.init();
    y_AxisGlobal.setPositions({ {0.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f} });
    y_AxisGlobal.setColors({ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });

    z_AxisGlobal.init();
    z_AxisGlobal.setPositions({ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.5f} });
    z_AxisGlobal.setColors({ {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });
}

//Rendert das lokale Koordinatensystem bei (0,0,0)
void drawLocalCS() {

    x_AxisLocal.render(projection, view);
    y_AxisLocal.render(projection, view);
    z_AxisLocal.render(projection, view);

}

//Rendert das globale Koordinatensystem bei (0,0,0)
void drawGlobalCS() {

    x_AxisGlobal.render(projection, view);
    y_AxisGlobal.render(projection, view);
    z_AxisGlobal.render(projection, view);

}

//Rotationsmatrizen f�r Rotation um x/y/z - Achse
glm::mat3 getRotMatrix(float angle, glm::vec3 axis) {
    if (axis.x > 0.0f) {
        return {
            1, 0, 0,
            0, cos(angle), -sin(angle),
            0, sin(angle), cos(angle) };
    }
    if (axis.y > 0.0f) {
        return {cos(angle), 0, sin(angle),
            0, 1, 0,
            -sin(angle), 0, cos(angle)};
    }
    if (axis.z > 0.0f) {
        return { cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0,
        0, 0, 1 };
    }
    return glm::mat3(1.0f);
}

//Rotiert die Kugel um das lokale Koordinatensystem
//Richtet sich nach den lokalen Achsen, welche durch globale Rotation ge�ndert werden
void rotateAroundLocalCS(float angle, glm::vec3 axis) {
    axis = glm::normalize(axis);

    glm::mat3 rotationMatrix = glm::mat3(1.0f);

    float cosTheta = cos(angle);
    float sinTheta = sin(angle);

    rotationMatrix[0][0] = cosTheta + (1 - cosTheta) * axis.x * axis.x;
    rotationMatrix[0][1] = (1 - cosTheta) * axis.x * axis.y - sinTheta * axis.z;
    rotationMatrix[0][2] = (1 - cosTheta) * axis.x * axis.z + sinTheta * axis.y;
    rotationMatrix[1][0] = (1 - cosTheta) * axis.y * axis.x + sinTheta * axis.z;
    rotationMatrix[1][1] = cosTheta + (1 - cosTheta) * axis.y * axis.y;
    rotationMatrix[1][2] = (1 - cosTheta) * axis.y * axis.z - sinTheta * axis.x;
    rotationMatrix[2][0] = (1 - cosTheta) * axis.z * axis.x - sinTheta * axis.y;
    rotationMatrix[2][1] = (1 - cosTheta) * axis.z * axis.y + sinTheta * axis.x;
    rotationMatrix[2][2] = cosTheta + (1 - cosTheta) * axis.z * axis.z;

    for (Triangle* t : faces) {
        glm::vec3 points[3];
        glBindBuffer(GL_ARRAY_BUFFER, t->positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        t->setPositions({ rotationMatrix * points[0], rotationMatrix * points[1], rotationMatrix * points[2] });
    }
}

//Rotiert die Kugel um das globale Koordinatensystem
//Rotiert dabei die lokalen Koordinatenachsen mit
void rotateAroundGlobalCS(float angle, glm::vec3 axis) {
    glm::mat3 rotationMatrix = getRotMatrix(angle, axis);
    for (Triangle* t : faces) {
        glm::vec3 points[3];
        glBindBuffer(GL_ARRAY_BUFFER, t->positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        t->setPositions({ rotationMatrix * points[0], rotationMatrix * points[1], rotationMatrix * points[2] });
    }

    glm::vec3 xAxisPoints[2];
    glBindBuffer(GL_ARRAY_BUFFER, x_AxisLocal.positionBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(xAxisPoints), xAxisPoints);

    glm::vec3 yAxisPoints[2];
    glBindBuffer(GL_ARRAY_BUFFER, y_AxisLocal.positionBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(yAxisPoints), yAxisPoints);

    glm::vec3 zAxisPoints[2];
    glBindBuffer(GL_ARRAY_BUFFER, z_AxisLocal.positionBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(zAxisPoints), zAxisPoints);

    x_AxisLocal.setPositions({ {0.0f,0.0f,0.0f}, rotationMatrix * xAxisPoints[1] });
    y_AxisLocal.setPositions({ {0.0f,0.0f,0.0f}, rotationMatrix * yAxisPoints[1] });
    z_AxisLocal.setPositions({ {0.0f,0.0f,0.0f}, rotationMatrix * zAxisPoints[1] });
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
    initLocalCS();
    initGlobalCS();
    approximateSphere();
    
    return true;
}
/*
 Rendering.
 */

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cs_switch == 0 ? drawGlobalCS() : drawLocalCS();
    
    for (Triangle* t : faces) {
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
    glm::vec3 xAxisPoints[2];
    glm::vec3 yAxisPoints[2];
    glm::vec3 zAxisPoints[2];
    switch (keycode) {
    case 27: // ESC
        glutDestroyWindow(glutID);
        return;

    case '+':
        // do something
        break;
    case '-':
        // do something
    case 'x':
        rotateAroundGlobalCS(glm::radians(1.0f), { 1.0f, 0.0f, 0.0f }); //Feste globale Achse
        break;
    case 'y':
        rotateAroundGlobalCS(glm::radians(1.0f), { 0.0f, 1.0f, 0.0f }); //Feste globale Achse
        break;
    case 'z':
        rotateAroundGlobalCS(glm::radians(1.0f), { 0.0f, 0.0f, 1.0f }); //Feste globale Achse
        break;
    case 'X':
        glBindBuffer(GL_ARRAY_BUFFER, x_AxisLocal.positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(xAxisPoints), xAxisPoints); 
        rotateAroundLocalCS(glm::radians(1.0f), xAxisPoints[1]); //variierende lokale Achse
        break;
    case 'Y':
        glBindBuffer(GL_ARRAY_BUFFER, y_AxisLocal.positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(yAxisPoints), yAxisPoints); 
        rotateAroundLocalCS(glm::radians(1.0f), yAxisPoints[1]); //variierende lokale Achse
        break;
    case 'Z':
        glBindBuffer(GL_ARRAY_BUFFER, z_AxisLocal.positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(zAxisPoints), zAxisPoints); 
        rotateAroundLocalCS(glm::radians(1.0f), zAxisPoints[1]); //variierende lokale Achse
        break;
    case 'k':
        cs_switch = !cs_switch;
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
