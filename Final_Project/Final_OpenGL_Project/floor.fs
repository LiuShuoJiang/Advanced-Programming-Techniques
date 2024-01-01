/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 6th, 2023
Description:
The fragment shader for the floor object
*/

#version 330 core
out vec4 FragColor;

in vec2 TexCoord; // Texture coordinates from vertex shader

// Texture sampler
uniform sampler2D texture1;

void main() 
{
    FragColor = texture(texture1, TexCoord);
    // FragColor = vec4(1.0);
}
