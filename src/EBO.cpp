// Modified from: https://github.com/VictorGordan/opengl-tutorials

#include "ui_library/EBO.h"

// Constructor that generates a Elements Buffer Object and links it to indices
EBO::EBO()
{
    glGenBuffers(1, &ID);
}

// Upload data to the element buffer
void EBO::Data(const std::vector<GLuint>& indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

// Update part of the buffer data
void EBO::SubData(const std::vector<GLuint>& indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
}

// Binds the EBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}