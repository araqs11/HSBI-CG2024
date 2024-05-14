#include "Sphere.h"
#include "Triangle.h"


Sphere::Sphere(cg::GLSLProgram& pg) : Object(pg) {
    n = 0;
    sphereRadius = 1;
    sphereCenter = { 0.0f,0.0f,0.0f };
    rotationAxis = { 0.0f,1.0f,0.0f };
    faces.clear();
    child = 0;
    axis = 0;
}

Sphere::~Sphere() {
    for (Triangle* t : faces) {
        delete t;
    }
}

void Sphere::translate(glm::vec3 translation) {
    for (Triangle* t : faces) {
        t->translate(translation);
    }
    sphereCenter = glm::translate(glm::mat4(1.0f), translation) * glm::vec4(sphereCenter, 1.0f);

    if (child != NULL) {
        child->translate(translation);
    }
}

void Sphere::setColor(glm::vec3 color) {
    for (Triangle* t : faces) {
        t->setColors({ color, color , color });
    }
}

void Sphere::setPosition(glm::vec3 position) {
    translate(position - sphereCenter);
}

void Sphere::init(float radius, glm::vec3 color) {
    approximateSphere();
    setRadius(radius);
    setColor(color);
}

void Sphere::render(glm::mat4x4 projection, glm::mat4x4 view) {
    for (Triangle* t : faces) {
        t->render(projection, view);
    }
    axis = new Line(program);
    axis->init();
    axis->setPositions({ sphereCenter - rotationAxis, sphereCenter + rotationAxis });
    axis->setColors({ {1.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f} });
    axis->render(projection, view);
    delete axis;
}

void Sphere::subdivideTriangle(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, int depth) {
    if (depth <= 0) {
        std::vector<glm::vec3> vertices = { v1, v2, v3 };
        Triangle* t = new Triangle(program);
        t->init();
        t->setPositions(vertices);
        faces.push_back(t);
    }
    else {
        float scale = depth / (depth + 1.0f);
        glm::vec3 L_LEFT = v1;
        glm::vec3 L_RIGHT = v1 + scale * (v2 - v1);
        glm::vec3 L_UP = v1 + scale * (v3 - v1);
        subdivideTriangle(L_LEFT, L_RIGHT, L_UP, depth - 1);

        glm::vec3 R_LEFT = v2 + scale * (v1 - v2);
        glm::vec3 R_RIGHT = v2;
        glm::vec3 R_UP = v2 + scale * (v3 - v2);
        subdivideTriangle(R_LEFT, R_RIGHT, R_UP, depth - 1);

        glm::vec3 M_LEFT = v3 + scale * (v1 - v3);
        glm::vec3 M_RIGHT = v3 + scale * (v2 - v3);
        glm::vec3 M_UP = v3;
        subdivideTriangle(M_LEFT, M_RIGHT, M_UP, depth - 1);
    }
}

void Sphere::approximateSphere() {
    for (Triangle* t : faces) {
        delete t;
    }
    faces.clear();

    subdivideTriangle(glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);
    subdivideTriangle(glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, n);


    for (int i = 0; i < faces.size(); i++) {
        glm::vec3 points[3];
        glBindBuffer(GL_ARRAY_BUFFER, faces[i]->positionBuffer);
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        glm::vec3 p1 = glm::normalize(points[0]) * sphereRadius;
        glm::vec3 p2 = glm::normalize(points[1]) * sphereRadius;
        glm::vec3 p3 = glm::normalize(points[2]) * sphereRadius;

        faces[i]->setPositions({ p1, p2, p3 });
    }
}

void Sphere::rotateAround(float angle, glm::vec3 axis, glm::vec3 center, bool calledByParent) {

    for (Triangle* t : faces) {
        t->rotate(angle, axis, GL_FALSE, center);
    }
    glm::mat4 translateToOrigin = glm::translate(-center);
    glm::mat4 rotation = glm::rotate(glm::radians(angle), axis);
    glm::mat4 translateBack = glm::translate(center);
    glm::mat4 rotationMatrix = translateBack * rotation * translateToOrigin;

    sphereCenter = rotationMatrix * glm::vec4(sphereCenter, 1.0f);
    if (calledByParent) {
        rotationAxis = rotationMatrix * glm::vec4(rotationAxis, 1.0f);
    }
    if (child != NULL) {
        child->rotateAround(angle, axis, center, GL_TRUE);
    }
}

void Sphere::resetRotation() {
    //TODO
}

void Sphere::setRadius(float newRadius) {
    sphereRadius = newRadius;
    approximateSphere();
}

void Sphere::setSubdivisions(unsigned int subdivisions) {
    n = subdivisions;
    approximateSphere();
}

glm::vec3 Sphere::getSphereCenter() {
    return sphereCenter;
}

glm::vec3 Sphere::getRotationAxis() {
    return rotationAxis;
}

void Sphere::setRotationAxis(glm::vec3 axis) {
    rotationAxis = axis;
}


Sphere* Sphere::getChild() {
    return child;
}

void Sphere::setChild(Sphere* ch) {
    child = ch;
}