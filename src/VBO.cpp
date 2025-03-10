/*
*  Modified from : https://github.com/VictorGordan/opengl-tutorials
*/

#include "ui_library/VBO.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO()
{
	glGenBuffers(1, &ID);  // Creates the name (populates ID)
}

// Update existing buffer to avoid having to create a new one
void VBO::SubData(const std::vector<GLfloat>& vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat), vertices.data());
}

// Make the buffer larger to accomodate new verticies
void VBO::Data(const std::vector<GLfloat>& vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}

// Binds the VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
