#pragma once
#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

class Triangle : public Object {
public:
	Triangle(cg::GLSLProgram& pg);
	~Triangle();
	void setPositions(std::vector<glm::vec3> positions);
	void setColors(std::vector<glm::vec3> positions);
	void setIndices(std::vector<GLushort> indices);
	void rotate(float angle, glm::vec3 axis);
	void init();
	void render(glm::mat4x4 projection, glm::mat4x4 view);
};
