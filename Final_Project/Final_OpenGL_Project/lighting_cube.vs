/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 6th, 2023
Description:
The vertex shader for the light cube
*/

#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
