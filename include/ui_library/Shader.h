// Copyright (c) 2025 Thomas Groom

#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include "stb_image.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string get_file_contents(const char* filename);

class Shader
{
	public:
        // material vars
        GLenum renderType = GL_TRIANGLES;

        // state
		GLuint ID;
        // constructor
		Shader();
        ~Shader();
		void Set(const char* vertexFile, const char* fragmentFile);
		
        //Shader() {}
        // sets the current shader as active
        Shader &Bind();
        void Unbind();
        // compiles the shader from given source code
        void    Compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource = nullptr); // note: geometry source code is optional 
        // utility functions
        void    SetFloat    (const char *name, float value, bool useShader = false);
        void    SetInteger  (const char *name, int value, bool useShader = false);
        void    SetUInteger (const char *name, unsigned int value, bool useShader = false);
        void    SetVector2f (const char *name, float x, float y, bool useShader = false);
        void    SetVector2f (const char *name, const glm::vec2 &value, bool useShader = false);
        void    SetVector3f (const char *name, float x, float y, float z, bool useShader = false);
        void    SetVector3f (const char *name, const glm::vec3 &value, bool useShader = false);
        void    SetVector4f (const char *name, float x, float y, float z, float w, bool useShader = false);
        void    SetVector4f (const char *name, const glm::vec4 &value, bool useShader = false);
        void    SetMatrix4  (const char *name, const glm::mat4 &matrix, bool useShader = false);

};



#endif