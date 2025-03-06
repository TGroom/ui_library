// Modified from: https://github.com/VictorGordan/opengl-tutorials

#pragma once

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO();//GLuint* indices, GLsizeiptr size);
	// Set new data in the EBO
	void Data(const std::vector<GLuint>& indices);
	// Edit existing EBO data
	void SubData(const std::vector<GLuint>& indices);
	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Deletes the EBO
	~EBO() {
		glDeleteBuffers(1, &ID);
	}
};
