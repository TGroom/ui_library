// Copyright (c) 2025 Thomas Groom

#pragma once

#include <map>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>

#include "Shader.h"
#include "Utils.h"
#include "VAO.h"
#include "VBO.h"
#include FT_FREETYPE_H


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // size of glyph
    glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
    unsigned int Advance;   // horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class Text
{
    public:
        enum Align {
            LEFT = 0x1,
            CENTER = 0x2,
            RIGHT = 0x4,
            TOP = 0x10,
            MIDDLE = 0x20,
            BOTTOM = 0x40,

            LEFT_TOP = LEFT | TOP,
            LEFT_MIDDLE = LEFT | MIDDLE,
            LEFT_BOTTOM = LEFT | BOTTOM,
            CENTER_TOP = CENTER | TOP,
            CENTER_MIDDLE = CENTER | MIDDLE,
            CENTER_BOTTOM = CENTER | BOTTOM,
            RIGHT_TOP = RIGHT | TOP,
            RIGHT_MIDDLE = RIGHT | MIDDLE,
            RIGHT_BOTTOM = RIGHT | BOTTOM
        };

        // constructor
        Text(std::string font, unsigned int fontSize);
        ~Text(){};
        // pre-compiles a list of characters from the given font
        void Load(std::string font, unsigned int fontSize);
        glm::ivec2 boundingBox(std::wstring text);
        // renders a string of text using the precompiled list of characters
        float RenderText(std::wstring text, Boundary textContainer, float z = 0.0f, Align align = CENTER_MIDDLE, Colour color = Colour(1.0f, 1.0f, 1.0f), bool truncate = true, bool selectable = false, int selectionStart = 0, int selectionEnd = 0, int caretPos = -1);
        unsigned int mFontSize = 12;

        float getTextHeight(const std::wstring& text, int containerWidth);

    private:
        int getTextWidth(const std::wstring& text);
        void truncateText(std::wstring& text, int maxWidth);

        // holds a list of pre-compiled Characters
        std::map<char, Character> Characters; 
        // shader used for text rendering
        Shader TextShader;
        // render state
        VAO VAO_Text;
        VBO VBO_Text;

        Shader HighlightShader;
        VAO VAO_Quad;
        VBO VBO_Quad;
        
        Colour selectionColor = Colour(0.2f, 0.4f, 0.8f);

        std::vector<std::wstring> lines;

};
