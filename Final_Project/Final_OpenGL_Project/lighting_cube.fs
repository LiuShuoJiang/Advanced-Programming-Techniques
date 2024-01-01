/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 6th, 2023
Description:
The fragment shader for the light cube
*/

#version 330 core
out vec4 FragColor;

uniform vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
