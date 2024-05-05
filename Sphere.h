#pragma once
#pragma once
#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

class Sphere : public Object {
public:
	Sphere(cg::GLSLProgram& pg);
	~Sphere();
	int indicesCount;
	void setPositions(std::vector<glm::vec3> positions);
	void setColors(std::vector<glm::vec3> positions);
	void setIndices(std::vector<GLushort> indices);
	void rotate(float angle, glm::vec3 axis);
	void init(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<GLushort> indices);
	void render(glm::mat4x4 projection, glm::mat4x4 view);
};

