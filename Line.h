#pragma once
#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

class Line : public Object {
public:
	Line(cg::GLSLProgram& pg);
	~Line();
	void setPositions(std::vector<glm::vec3> positions);
	void setColors(std::vector<glm::vec3> positions);
	void rotate(float angle, glm::vec3 axis);
	void init();
	void render(glm::mat4x4 projection, glm::mat4x4 view);
};