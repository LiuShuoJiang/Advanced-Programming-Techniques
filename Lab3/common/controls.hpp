/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: 10/26/2023
Description:
Control class to set movements
*/

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

//NEW CODE
extern bool lighting;

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
//NEW CODE
void callbackForKeys(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif