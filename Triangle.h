#pragma once
#include "Object.h"

class Triangle : public Object {
public:
	Triangle(cg::GLSLProgram& pg);
	~Triangle();
	void setPositions(std::vector<glm::vec3> positions);
	void setColors(std::vector<glm::vec3> positions);
	void setIndices(std::vector<GLushort> indices);
	void init();
	void render(glm::mat4x4 projection, glm::mat4x4 view);
};
