#include "Sphere.h"

Sphere::Sphere(cg::GLSLProgram& pg) : Object(pg), sphereRadius(1.0f), indicesCount(0){
    std::vector<Triangle*> faces;
}

Sphere::~Sphere() {

}
void Sphere::setPositions(std::vector<glm::vec3> positions) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

	GLuint programId = program.getHandle();
	GLuint pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
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

void Sphere::rotate(float angle, glm::vec3 axis) {
	model = glm::rotate(model, glm::radians(angle), axis);
}

void Sphere::init(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<GLushort> indices) {
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	indicesCount = indices.size();

	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

}

void Sphere::render(glm::mat4x4 projection, glm::mat4x4 view) {
	// Create mvp.
	glm::mat4x4 mvp = projection * view * model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Setzt den Rendermodus auf Linien
	// Bind vertex array object so we can render the 1 triangle.
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Setzt den Rendermodus auf Fill
}

void Sphere::subdivideTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth) {
    //Sobald die Erforderte tiefe erreicht wurde werden die bis hierhin errechneten Punkte in Tatsächliche dreiecks Objekte eingeschrieben,
    // und dann im globalen vector "faces" abgespeichert
    if (depth <= 0) {
        std::vector<glm::vec3> vertices = { v1, v2, v3 };
        Triangle* t = new Triangle(program);
        t->init();
        t->setPositions(vertices);
        faces.push_back(t);
    }
    else { // Teilt ein Dreieck in 3 Dreiecke der vorherigen Tiefe auf.Dies geschieht dadurch, dass eine Ecke des originalen Dreiecks (P1),
        // mit zwei errechneten Punkten die auf den Vektoren von P1 zu P2 und P1 zu P3 liegen. Danach werden diese errechneten Dreiecke rekursiv wieder aufgeteilt.
        // hierbei ist zu beachten, dass die neu errechneten Punkte der Dreiecke noch nicht die passende Entfernung zum Mittelpunkt haben. Diese wird später ausgerechnet
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

void Sphere::approximateSphere(int depth,float radius) {
    std::vector<glm::vec3> vectors;
    std::vector<glm::vec3> color;
    std::vector<GLushort> indices;

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
        color.push_back({ 255.0f,255.0f,0.0f });
        color.push_back({ 255.0f,255.0f,0.0f });
        color.push_back({ 255.0f,255.0f,0.0f });
        indices.push_back(3.0f * i);
        indices.push_back(3.0f * i + 1.0f);
        indices.push_back(3.0f * i + 2.0f);

    }
    init(vectors, color, indices);
}

void Sphere::scaleSphere(float amount, glm::vec3 centerPoint) {
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    if (sphereRadius + amount < glm::distance(centerPoint, glm::vec3(0.0f, 0.0f, 0.0f)) && sphereRadius + amount>0.5) {
        float percentile = 1.0f + (amount / sphereRadius);
        sphereRadius += amount;
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(percentile, percentile, percentile));
        model = scale * model;
    }
}