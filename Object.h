#pragma once

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "GLSLProgram.h"

class Object
{
public:
    Object(cg::GLSLProgram& pg);
    ~Object();

    GLuint vao;        // vertex-array-object ID

    GLuint positionBuffer; // ID of vertex-buffer: position
    GLuint colorBuffer;    // ID of vertex-buffer: color

    GLuint indexBuffer;    // ID of index-buffer

    glm::mat4x4 model; // model matrix

    cg::GLSLProgram& program;
};