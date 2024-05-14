#pragma once
#include "Sphere.h"
class Planet : public Sphere
{
public:
	Planet(cg::GLSLProgram& pg);
	~Planet();
	glm::vec3 center;
	std::vector<Planet> orbiting;
	void orbit(glm::vec3 around);
	void setCenter(glm::vec3 c);
	void addOrbiting(Planet p);
	void approximateSphere(int depth, float radius, std::vector<glm::vec3> colors);
};

