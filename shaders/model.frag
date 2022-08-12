// model.frag - Model fragment shader.
// Copyright (C) 2022 Trevor Last
#version 430 core

in vec2 fTexCoords;
in vec3 fVertexColor;

out vec4 FragColor;

uniform sampler2D tex;


void main()
{
    FragColor = vec4(fVertexColor, 1.0) * texture(tex, fTexCoords);
}
