#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 worldMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;

uniform bool hadMapNorm;
uniform sampler2D mapNorm;

out vec3 iPosition;
out vec3 iNormal;
out vec2 iTexCoord;


void main()
{
    iPosition = vec3(worldMatrix * vec4(Position, 1.0));
    iNormal = vec3(normalMatrix * vec4(Normal, 0.0));
    if(hadMapNorm)
        iNormal = vec3(normalMatrix * vec4(vec3(texture2D(mapNorm, TexCoord)), 0.0));
    iTexCoord = TexCoord;
    gl_Position = MVP * vec4(Position, 1.0);
} 