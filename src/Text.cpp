// Copyright (c) 2025 Thomas Groom


#include "ui_library/Text.h"


Text::Text(std::string font, unsigned int fontSize) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    TextShader.Set((std::string(UI_LIBRARY_RESOURCES_DIR) + "/shaders/Text.vert").c_str(), (std::string(UI_LIBRARY_RESOURCES_DIR) + "/shaders/Text.frag").c_str());

    Load(font, fontSize);
}


void Text::Load(std::string font, unsigned int fontSize)
{
    mFontSize = fontSize;
    Characters.clear();
    FT_Library ft;    
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, mFontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << (int)c << " ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
            );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    VAO_Text.Bind();
    VBO_Text.Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * 9, NULL, GL_DYNAMIC_DRAW);
    //VAO_Text.LinkAttrib(VBO_Text, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);
    VAO_Text.LinkAttrib(VBO_Text, 0, 3, GL_FLOAT, 9 * sizeof(float), (void*)0);
	VAO_Text.LinkAttrib(VBO_Text, 1, 2, GL_FLOAT, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_Text.LinkAttrib(VBO_Text, 2, 4, GL_FLOAT, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    VBO_Text.Unbind();
    VAO_Text.Unbind();
}

// TODO: Does not take into account text scale
glm::ivec2 Text::boundingBox(std::wstring text){
    glm::ivec2 box = {1, 10};
    std::wstring::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];
        box.x += ch.Advance >> 6;
        //if(ch.Size.y > box.y) box.y = ch.Size.y;
    }
    return box;
}

int Text::getTextWidth(const std::wstring& text) {
    int textWidth = 0;
    for (std::wstring::const_iterator c = text.begin(); c != text.end(); ++c) {
        Character ch = Characters[*c];
        textWidth += (ch.Advance >> 6); // Bitshift by 6 to convert from 1/64th pixels
    }
    return textWidth;
}

void Text::truncateText(std::wstring& text, int maxWidth) { 
    if (maxWidth <= 0) {
        text = L"";
        return;
    }

    if (text.empty()) return;

    int firstLetterWidth = getTextWidth(text.substr(0, 1));
    int ellipsisWidth = getTextWidth(L"...");
    
    // If maxWidth is smaller than the first letter, show nothing
    if (firstLetterWidth > maxWidth) {
        text = L"";
        return;
    }

    int textWidth = getTextWidth(text);

    // If text fits without truncation, leave it as is
    if (textWidth <= maxWidth) {
        return;
    }

    // Check if we can fit the first letter and "..."
    if (firstLetterWidth + ellipsisWidth <= maxWidth) {
        std::wstring truncatedText = text.substr(0, 1); // Keep the first letter
        int currentWidth = firstLetterWidth;

        for (size_t i = 1; i < text.size(); ++i) {
            Character ch = Characters[text[i]];
            int charWidth = ch.Advance >> 6;

            if (currentWidth + charWidth + ellipsisWidth > maxWidth) {
                truncatedText += L"...";
                text = truncatedText;
                return;
            }

            truncatedText += text[i];
            currentWidth += charWidth;
        }

        // If we reach here, all letters fit
        text = truncatedText;
        return;
    }

    // If not even the first letter and "..." fit, truncate to the maximum letters that fit
    std::wstring truncatedText = L"";
    int currentWidth = 0;

    for (size_t i = 0; i < text.size(); ++i) {
        Character ch = Characters[text[i]];
        int charWidth = ch.Advance >> 6;

        if (currentWidth + charWidth > maxWidth) {
            break;
        }

        truncatedText += text[i];
        currentWidth += charWidth;
    }

    text = truncatedText;
}

float Text::getTextHeight(const std::wstring& text, int containerWidth) {
    float lineWidth = 0;
    int lineCount = 1;

    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t c = text[i];
        if (c == L'\n') {
            lineCount++;
            lineWidth = 0;
            continue;
        }
        float charWidth = Characters[c].Advance >> 6;

        if (lineWidth + charWidth > containerWidth) {
            lineCount++;
            lineWidth = 0;
        }
        lineWidth += charWidth;
    }
    return lineCount * mFontSize;  // Return the text height
}

// TODO: CHECK THAT CHANGING THE FONT SIZE AFFECTS HOW MUCH IS TRUNCATED!!!!
float Text::RenderText(std::wstring text, Boundary textContainer, float z, Align align, Colour color, bool truncate, bool selectable, int selectionStart, int selectionEnd, int caretPos)
{
    if (!lines.empty()) {
        lines.clear();
    }

    if (truncate) {
        truncateText(text, textContainer.width);
        lines.push_back(text);
    }
    else {
        // Wrap text to fit within the width of the text container
        std::vector<int> lineStartIndices;
        std::wstring currentLine;
        float lineWidth = 0;
        int currentCharIndex = 0;

        for (size_t i = 0; i < text.size(); ++i)
        {
            wchar_t c = text[i];
            if (c == L'\n') 
            {
                lines.push_back(currentLine);
                lineStartIndices.push_back(currentCharIndex);
                currentLine.clear();
                lineWidth = 0;
                continue;
            }

            Character ch = Characters[c];
            float charWidth = ch.Advance >> 6; // Advance in pixels

            if (lineWidth + charWidth > textContainer.width && !currentLine.empty())
            {
                lines.push_back(currentLine);
                lineStartIndices.push_back(currentCharIndex);
                currentLine.clear();
                lineWidth = 0;
            }

            currentLine += c;
            lineWidth += charWidth;
            currentCharIndex++;
        }

        // Add the last line if not empty
        if (!currentLine.empty()) 
        {
            lines.push_back(currentLine);
            lineStartIndices.push_back(currentCharIndex);
        }
    }


    float textHeight = lines.size() * mFontSize;
    
    int wWidth = 0;
    int wHeight = 0;
    glfwGetFramebufferSize(G_WINDOW, &wWidth, &wHeight);
    // Activate the corresponding render state
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(wWidth), static_cast<float>(wHeight), 0.0f, -1.0f, 1.0f);
    TextShader.Bind();
    glUniformMatrix4fv(glGetUniformLocation(TextShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(TextShader.ID, "textColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    VAO_Text.Bind();

    float yOffset = textContainer.y + mFontSize - 1;
    if (align & BOTTOM) {
        yOffset += textContainer.height - mFontSize - 5;
    }
    else if (align & MIDDLE) {
        yOffset += (textContainer.height - mFontSize) / 2;
    }

    int globalCharIndex = 0;

    for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex)
    {
        const std::wstring& line = lines[lineIndex];
        int lineWidthPixels = getTextWidth(line);
        float xOffset = static_cast<float>(textContainer.x);

        // Handle alignment
        if (textContainer.width > 0) 
        {
            int blankSpace = (textContainer.width - lineWidthPixels);
            if (align & RIGHT)
            {
                xOffset += blankSpace;
            }
            else if (align & CENTER)
            {
                xOffset += blankSpace / 2;
            }
        }

        // Render each character in the line
        for (size_t i = 0; i < line.size(); ++i, ++globalCharIndex)
        {
            wchar_t c = line[i];
            Character ch = Characters[c];

            float xpos = xOffset + ch.Bearing.x;
            float ypos = yOffset + (ch.Size.y - ch.Bearing.y);
            float yposEnd = (wHeight - yOffset);

            float w = ch.Size.x;
            float h = ch.Size.y;

            float bgAlpha = 0.0f;
            // Determine if the character is selected
            bool isSelected = selectable && //selectionStart && selectionEnd && 
                              globalCharIndex >= std::min(selectionStart, selectionEnd) && 
                              globalCharIndex < std::max(selectionStart, selectionEnd);

            // Render the selection background if the character is selected
            if (isSelected) {
                bgAlpha = 0.7f;
            }

            //printf("%d, %d, %d, %d, %d\n", isSelected, globalCharIndex, selectionStart, selectionEnd);  // Debugging
             
            float caretAlpha = (globalCharIndex == caretPos) ? 1.0f : 0.0f;
            
            float wOffset = (ch.Advance >> 6);
            float yOffsetH = yOffset + 3.0f;

            // Define vertices
            float vertices[18][9] = {
                // Selection vertices
                { xOffset,           yOffsetH - mFontSize,   z + 3e-4, 2, 2, selectionColor.r, selectionColor.g, selectionColor.b, bgAlpha },
                { xOffset,           yOffsetH,               z + 3e-4, 2, 2, selectionColor.r, selectionColor.g, selectionColor.b, bgAlpha },
                { xOffset + wOffset, yOffsetH,               z + 3e-4, 2, 2, selectionColor.r, selectionColor.g, selectionColor.b, bgAlpha },
                { xOffset,           yOffsetH - mFontSize,   z + 3e-4, 2, 2, selectionColor.r, selectionColor.g, selectionColor.b, bgAlpha },
                { xOffset + wOffset, yOffsetH,               z + 3e-4, 2, 2, selectionColor.r, selectionColor.g, selectionColor.b, bgAlpha },
                { xOffset + wOffset, yOffsetH - mFontSize,   z + 3e-4, 2, 2, selectionColor.r, selectionColor.g, selectionColor.b, bgAlpha },

                // Character vertices
                { xpos,       ypos - h,   z + 6e-4, 0, 0, 0, 0, 0, 0 },
                { xpos,       ypos,       z + 6e-4, 0, 1, 0, 0, 0, 0 },
                { xpos + w,   ypos,       z + 6e-4, 1, 1, 0, 0, 0, 0 },
                { xpos,       ypos - h,   z + 6e-4, 0, 0, 0, 0, 0, 0 },
                { xpos + w,   ypos,       z + 6e-4, 1, 1, 0, 0, 0, 0 },
                { xpos + w,   ypos - h,   z + 6e-4, 1, 0, 0, 0, 0, 0 },

                // Caret vertices
                { xOffset,           yOffsetH - mFontSize,   z + 9e-4, 2, 2, 1, 1, 1, caretAlpha },
                { xOffset,           yOffsetH,               z + 9e-4, 2, 2, 1, 1, 1, caretAlpha },
                { xOffset + 1,       yOffsetH,               z + 9e-4, 2, 2, 1, 1, 1, caretAlpha },
                { xOffset,           yOffsetH - mFontSize,   z + 9e-4, 2, 2, 1, 1, 1, caretAlpha },
                { xOffset + 1,       yOffsetH,               z + 9e-4, 2, 2, 1, 1, 1, caretAlpha },
                { xOffset + 1,       yOffsetH - mFontSize,   z + 9e-4, 2, 2, 1, 1, 1, caretAlpha },

            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            VBO_Text.Bind();
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            VBO_Text.Unbind();
            glDrawArrays(GL_TRIANGLES, 0, 18);

            xOffset += wOffset; // Advance cursor for the next glyph
        }
        yOffset += mFontSize; // Move to the next line
    }

    VAO_Text.Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    TextShader.Unbind();

    return textHeight;
}


