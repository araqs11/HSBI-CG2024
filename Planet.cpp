#include "Planet.h"

Planet::Planet(cg::GLSLProgram& pg) : Sphere(pg),center(0){
	std::vector <Planet> orbiting;
}

Planet::~Planet() {
}

void Planet::orbit(glm::vec3 around) {
	glm::mat4x4 mat;

	mat = glm::translate(mat,(around));
	mat = glm::rotate(mat, glm::radians(0.01f), { 0.0f,1.0f,0.0f });
	mat = glm::translate(mat,-(around));


	model = mat*model;
}

void Planet::setCenter(glm::vec3 c) {
	center = c;
	model = glm::translate(model, center);
}

void Planet::addOrbiting(Planet p) {
	orbiting.push_back(p);
}

void Planet::approximateSphere(int depth, float radius, std::vector<glm::vec3> colors) {
    std::vector<glm::vec3> vectors;
    std::vector<glm::vec3> color;
    std::vector<GLushort> indices;
    sphereRadius = radius;

    for (Triangle* t : faces) {
        delete t;
    }
    faces.clear();
    //Starte werte für Sphere mit 8 Seiten
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, depth);
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, depth);
    subdivideTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, depth);
    subdivideTriangle({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, depth);
    subdivideTriangle({ 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, depth);
    subdivideTriangle({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, depth);
    subdivideTriangle({ 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, depth);
    subdivideTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, depth);

    //iteriert durch die abgespeicherten dreiecke durch und fixt die Position der Punkte durch anpassung der Entfernung zum mittelpunkt
    for (int i = 0; i < faces.size(); i++) {
        glm::vec3 points[3];
        glBindBuffer(GL_ARRAY_BUFFER, faces[i]->positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        //normalisiert den Vector zu der länge 1 und multiplieziertdann wieder mit den gewünschten Radius
        glm::vec3 p1 = glm::normalize(points[0]) * sphereRadius;
        glm::vec3 p2 = glm::normalize(points[1]) * sphereRadius;
        glm::vec3 p3 = glm::normalize(points[2]) * sphereRadius;

        //Speichert die neuen Positionen ab

        vectors.push_back(p1);
        vectors.push_back(p2);
        vectors.push_back(p3);
        color.push_back(colors[0]);
        color.push_back(colors[1]);
        color.push_back(colors[2]);
        indices.push_back(3.0f * i);
        indices.push_back(3.0f * i + 1.0f);
        indices.push_back(3.0f * i + 2.0f);

    }
    init(vectors, color, indices);
}