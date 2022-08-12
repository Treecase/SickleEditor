// model.vert - Model vertex shader.
// Copyright (C) 2022 Trevor Last
#version 430 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec2 vTexCoords;
layout(location=2) in vec3 vVertexColor;

out vec2 fTexCoords;
out vec3 fVertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    fTexCoords = vTexCoords;
    fVertexColor = vVertexColor;
}
