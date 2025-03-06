//#version 330 core
//layout (location = 0) in vec2 aPos;
//layout (location = 1) in vec2 aTexCoord;
//
//out vec2 TexCoords;
//
//uniform mat4 projection;
//
//void main()
//{
//    gl_Position = projection * vec4(aPos, 0.0, 1.0);
//    TexCoord = aTexCoord;
//}

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aBackgroundColor;  // Background colour with alpha

out vec2 TexCoords;
out vec4 BackgroundColor;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 1.0);
    TexCoords = aTexCoord;
    BackgroundColor = aBackgroundColor;  // Pass the background colour to the fragment shader
}


//layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
