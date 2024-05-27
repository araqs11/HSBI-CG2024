#include "Sphere.h"


Sphere::Sphere(cg::GLSLProgram& pg) : Object(pg) {
	center = { 0.0f, 0.0f , 0.0f };
	farbe = 0;
}

Sphere::~Sphere() {

}
void Sphere::setPosition(glm::vec3 position) {
	for (Triangle* t : faces) {
		t->translate(position);
	}
	center = glm::translate(glm::mat4(1.0f), position) * glm::vec4(center, 1.0f);
}

void Sphere::setColors(std::vector<glm::vec3> colors) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	GLuint programId = program.getHandle();
	GLuint pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

void Sphere::setIndices(std::vector<GLushort> indices) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}


void Sphere::rotate(float angle, glm::vec3 axis, glm::vec3 ursprung) {
	// Zum Ursprung bewegen, dann Rotieren, dann zurück bewegen
	if (ursprung == center) {

		for (Triangle* t : faces) {
			t->rotate(angle, axis);
		}

	}
	else {	
		glm::mat4 rotMatrix(1.0f);

		glm::mat4 translationZumUrsprung = glm::translate(glm::mat4(1.0f), -ursprung);
		glm::mat4 rotationUmUrsprung = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
		glm::mat4 translationZurück = glm::translate(glm::mat4(1.0f), ursprung);

		rotMatrix = translationZurück * rotationUmUrsprung * translationZumUrsprung;

		center = rotMatrix * glm::vec4(center, 1.0f);

		/*
		std::cout << center[0] << " " << center[1] << " " << center[2] << std::endl;
		std::cout << rotMatrix[0][0] << " " << rotMatrix[0][1] << " " << rotMatrix[0][2] << " " << rotMatrix[0][2] << std::endl;
		std::cout << rotMatrix[1][0] << " " << rotMatrix[1][1] << " " << rotMatrix[1][2] << " " << rotMatrix[1][2] << std::endl;
		std::cout << rotMatrix[2][0] << " " << rotMatrix[2][1] << " " << rotMatrix[2][2] << " " << rotMatrix[2][2] << std::endl;
		std::cout << rotMatrix[3][0] << " " << rotMatrix[3][1] << " " << rotMatrix[3][2] << " " << rotMatrix[3][2] << std::endl;
		std::cout << "--------------" << std::endl;
		*/

		for (Triangle* t : faces) {
			t->model = rotMatrix * t->model;

		}

		translationZumUrsprung = glm::translate(glm::mat4(1.0f), -center);
		rotationUmUrsprung = glm::rotate(glm::mat4(1.0f), glm::radians(-angle), axis);
		translationZurück = glm::translate(glm::mat4(1.0f), center);

		rotMatrix = translationZurück * rotationUmUrsprung * translationZumUrsprung;
		for (Triangle* t : faces) {
			t->model = rotMatrix * t->model;
		}
	}
}


void Sphere::init(float radius, int n, int farbe) {
	this->farbe = farbe;
	approximateSphere(radius, n);

}

void Sphere::render(glm::mat4x4 projection, glm::mat4x4 view) {
	for (Triangle* t : faces) {
		t->render(projection, view);
	}
}

void Sphere::subdivideTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth) {

	if (depth <= 0) {
		std::vector<glm::vec3> vertices = { v1, v2, v3 };
		Triangle* t = new Triangle(program);
		t->init();
		if (farbe == 1) {
			t->setColors({{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f , 1.0f} });
		}
		else if (farbe == 2) {
			t->setColors({ {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f , 1.0f} });

		}
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

void Sphere::approximateSphere(float radius, int n) {
	for (Triangle* t : faces) {
		delete t;
	}
	faces.clear();
	//Starte werte für Sphere mit 8 Seiten
	subdivideTriangle({ 0.0f, 0.0f, 1.0f }, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f , 0.0f}, n);
	subdivideTriangle({ 0.0f , 0.0f, 1.0f }, { -1.0f , 0.0f, 0.0f }, { 0.0f , -1.0f , 0.0f }, n);
	subdivideTriangle({ 1.0f , 0.0f, 0.0f }, { 0.0f , 0.0f, -1.0f }, { 0.0f , -1.0f , 0.0f }, n);
	subdivideTriangle({ -1.0f , 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f , -1.0f , 0.0f }, n);
	subdivideTriangle({ 0.0f , 0.0f, 1.0f }, { -1.0f , 0.0f, 0.0f }, { 0.0f , 1.0f , 0.0f }, n);
	subdivideTriangle({ -1.0f , 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f , 1.0f , 0.0f }, n);
	subdivideTriangle({ 0.0f , 0.0f, -1.0f }, { 1.0f , 0.0f, 0.0f }, { 0.0f, 1.0f , 0.0f }, n);
	subdivideTriangle({ 1.0f , 0.0f, 0.0f }, { 0.0f , 0.0f, 1.0f }, { 0.0f , 1.0f , 0.0f }, n);


	//iteriert durch die abgespeicherten dreiecke durch und fixt die Position der Punkte durch anpassung der Entfernung zum mittelpunkt
	for (int i = 0; i < faces.size(); i++) {
		glm::vec3 points[3];
		glBindBuffer(GL_ARRAY_BUFFER, faces[i]->positionBuffer);
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);


		//normalisiert den Vector zu der länge 1 und multiplieziertdann wieder mit den gewünschten Radius
		glm::vec3 p1 = glm::normalize(points[0]) * radius;
		glm::vec3 p2 = glm::normalize(points[1]) * radius;
		glm::vec3 p3 = glm::normalize(points[2]) * radius;
		//Speichert die neuen Positionen ab
		faces[i]->setPositions({ p1, p2, p3 });
	}
}