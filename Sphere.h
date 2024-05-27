#pragma once
#include "Object.h"
#include "Triangle.h"
#include "Line.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Sphere : public Object {
public:
	Sphere(cg::GLSLProgram& pg);
	~Sphere();
	void translate(glm::vec3 translation);
	void setColor(glm::vec3 color);
	void setPosition(glm::vec3 position);
	void init(float radius, glm::vec3 color);
	void render(glm::mat4x4 projection, glm::mat4x4 view);

	void subdivideTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, int depth);
	void approximateSphere();
	void rotateAround(float angle, glm::vec3 axis, glm::vec3 center);
	void rotateLocal(float angle, glm::vec3 axis);
	void rotateAroundSelf(float angle, glm::vec3 axis);
	void resetRotation();

	void setRadius(float newRadius);
	void setSubdivisions(unsigned int subdivisions);

	glm::vec3 getSphereCenter();
	glm::vec3 getRotationAxis();
	void setRotationAxis(glm::vec3 axis);
	Sphere* getChild();
	void setChild(Sphere* ch);

	std::vector<Triangle*> faces;
	unsigned int n;
	float sphereRadius;
	glm::vec3 sphereCenter;
	glm::vec3 rotationAxis;
	Line* axis;
	Sphere* child;
};

