#version 330 core

in vec2 iTexCoord;

uniform sampler2D mapKd;

out vec4 FragColor;


void main()
{
    FragColor = texture2D(mapKd, vec2(iTexCoord.x, 1.0 - iTexCoord.y));
}

