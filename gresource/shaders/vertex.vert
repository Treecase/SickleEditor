// vertex.vert - ScreenQuad vertex shader.
// Copyright (C) 2022 Trevor Last
#version 430 core

layout(location=0) in vec3 vPos;
layout(location=1) in vec2 vTexCoords;

out vec2 fTexCoords;


void main()
{
    fTexCoords = vTexCoords;
    gl_Position = vec4(vPos, 1.0);
}
