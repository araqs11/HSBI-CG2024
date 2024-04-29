#include "Object.h"

Object::Object(cg::GLSLProgram& pg) : vao(0), positionBuffer(0), colorBuffer(0), indexBuffer(0), program(pg) {}

Object::~Object() { // GL context must exist on destruction
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &positionBuffer);
}