#pragma once
#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Triangle : public Object {
public:
	Triangle(cg::GLSLProgram& pg);
	~Triangle();
	void setPositions(std::vector<glm::vec3> positions);
	void setColors(std::vector<glm::vec3> positions);
	void setIndices(std::vector<GLushort> indices);
	void rotate(float angle, glm::vec3 axis, bool local, glm::vec3 rotationCenter);
	void translate(glm::vec3 translation);
	void init();
	void render(glm::mat4x4 projection, glm::mat4x4 view);
};


