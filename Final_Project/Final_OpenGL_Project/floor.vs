/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 6th, 2023
Description:
The vertex shader for the floor object
*/

#version 330 core
layout (location = 0) in vec3 aPos;   // Position variable
layout (location = 1) in vec2 aTexCoord; // Texture coordinate variable

out vec2 TexCoord; // Texture coordinate output to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
