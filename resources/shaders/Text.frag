#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec4 BackgroundColor;

uniform sampler2D text;
uniform vec3 textColor;


void main()
{
    if (TexCoords.x < 0.0 || TexCoords.x > 1.0 || TexCoords.y < 0.0 || TexCoords.y > 1.0)
    {
        FragColor = BackgroundColor;
        return;
    }

    // Sample the texture normally if UV coordinates are valid
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    FragColor = vec4(textColor, 1.0) * sampled + BackgroundColor;
}