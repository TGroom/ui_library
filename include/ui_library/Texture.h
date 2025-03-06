/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <glad/glad.h>
#include <filesystem>
#include <queue>
#include <mutex>
#include <thread>
#include <glm/glm.hpp>
#include <ui_library/stb_image.h>
#include <ui_library/stb_image_resize2.h>
#include <condition_variable>

#include "ui_library/Config.h"
#include "Utils.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
    unsigned int ID = 0;

    unsigned int width, height;
    unsigned int internalFormat;
    unsigned int imageFormat;
    unsigned int wrapS;
    unsigned int wrapT;
    unsigned int filterMin;
    unsigned int filterMax;

    Texture2D(GLenum internalFormat, GLenum imageFormat, const std::filesystem::path& file, glm::vec2 boundingBox);
    ~Texture2D() {
        if (this->ID != 0) {
            glDeleteTextures(1, &this->ID);
        }
    }

    void loadTextureFromFileAsync(const std::filesystem::path& file, glm::vec2 boundingBox);
    void DrawSprite(glm::vec2 position, glm::vec2 desiredSize, float z = 0.0f, float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

    glm::vec2 mDesiredSize = glm::vec2(20.0f, 20.0f);
    glm::vec2 mFitSize = glm::vec2(20.0f, 20.0f);

private:
    void Bind() const;
    void Unbind() const;
    void Create(GLuint texWidth, GLuint texHeight, const unsigned char* pixelData);
    void processTextureQueue();

    Shader spriteShader;
    VAO VAO1;
    VBO VBO1;

    std::queue<std::tuple<unsigned char*, glm::vec2>> textureQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
};

#endif