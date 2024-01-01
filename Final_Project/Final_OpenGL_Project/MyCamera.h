/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 6th, 2023
Description:
This is the header file of MyCamera class. It defines the camera position and the camera movement.
*/

#ifndef MY_CAMERA_H
#define MY_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum KeyBoard_Pressing
{
	PRESS_KEY_LEFT,
    PRESS_KEY_RIGHT,
    PRESS_KEY_UP,
    PRESS_KEY_DOWN,
    PRESS_KEY_U,
    PRESS_KEY_D
};

class MyCamera
{
public:
    glm::vec3 Target;  // Center of the scene
    glm::vec3 Position;
    glm::vec3 WorldUp;

    float Distance;
    float HorizontalAngle;
    float VerticalAngle;

    MyCamera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), float distance = 3.0f, float horizontalAngle = 0.0f, 
        float verticalAngle = 0.0f, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
	{
        Target = target;
        Distance = distance;
        HorizontalAngle = horizontalAngle;
        VerticalAngle = verticalAngle;
        WorldUp = worldUp;
        updateCameraPosition();
    }

    glm::mat4 GetViewMatrix()
	{
        return glm::lookAt(Position, Target, WorldUp);
    }

    void ProcessKeyboard(KeyBoard_Pressing key, float deltaTime)
	{
        float angleSpeed = 1.0f; // Angular speed
        float zoomSpeed = 2.4f;  // Zoom speed

        // Rotate left/right
        if (key == PRESS_KEY_LEFT) 
        {
            HorizontalAngle -= angleSpeed * deltaTime;
        }
        if (key == PRESS_KEY_RIGHT) 
        {
            HorizontalAngle += angleSpeed * deltaTime;
        }

        // Zoom in/out
        if (key == PRESS_KEY_UP) 
        {
            Distance -= zoomSpeed * deltaTime;
        }
        if (key == PRESS_KEY_DOWN) 
        {
            Distance += zoomSpeed * deltaTime;
        }

        // Rotate up/down
        if (key == PRESS_KEY_U) 
        {
            VerticalAngle += angleSpeed * deltaTime;
        }
        if (key == PRESS_KEY_D) 
        {
            VerticalAngle -= angleSpeed * deltaTime;
        }

        // Preventing from zooming too close or too far
        if (Distance < 1.0f) 
        {
            Distance = 1.0f;
        }
        if (Distance > 10.0f) 
        { // Adjust max distance as needed
            Distance = 10.0f;
        }

        // Clamp the vertical angle to prevent flip-over at the poles
        if (VerticalAngle < -glm::half_pi<float>() + 0.1f) 
        {
            VerticalAngle = -glm::half_pi<float>() + 0.1f;
        }
        if (VerticalAngle > glm::half_pi<float>() - 0.1f) 
        {
            VerticalAngle = glm::half_pi<float>() - 0.1f;
        }

        updateCameraPosition();
    }

private:
    void updateCameraPosition()
	{
        Position.x = Target.x + Distance * cos(VerticalAngle) * sin(HorizontalAngle);
        Position.y = Target.y + Distance * sin(VerticalAngle);
        Position.z = Target.z + Distance * cos(VerticalAngle) * cos(HorizontalAngle);
    }
};

#endif // MY_CAMERA_H

