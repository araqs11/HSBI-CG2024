#pragma once
#pragma once
#include "Object.h"
#include "Triangle.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Sphere : public Object {
public:
	Sphere(cg::GLSLProgram& pg);
	~Sphere();
	void setPosition(glm::vec3 position);
	void setColors(std::vector<glm::vec3> colors);
	void setIndices(std::vector<GLushort> indices);
	void rotate(float angle, glm::vec3 axis, glm::vec3 ursprung);
	void init(float radius, int n, int farbe);
	void render(glm::mat4x4 projection, glm::mat4x4 view);
	void subdivideTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth);
	void approximateSphere(float radius, int n);
	std::vector<Triangle*> faces;
	int farbe;
	glm::vec3 center;
};

