// Modified from: https://github.com/VictorGordan/opengl-tutorials

#pragma once

#include <glad/glad.h>
#include <vector>

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO();
	// Set new data in the VBO
	void Data(const std::vector<GLfloat>& vertices);
	// Edit existing VBO data
	void SubData(const std::vector<GLfloat>& vertices);
	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	~VBO() {
		glDeleteBuffers(1, &ID);
	}
};
