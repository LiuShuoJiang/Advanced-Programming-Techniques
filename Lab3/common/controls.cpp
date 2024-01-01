/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: 10/26/2023
Description:
Control class to set movements
*/

// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

const float PI = 3.141592653589793f;

/*
// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
*/

// Initial position
glm::vec3 position = glm::vec3(0, 0, 0);
glm::vec3 top = glm::vec3(0, 0, 1);
glm::vec3 origin(0, 0, 0);

float phi = 0.0f;
float phiDiff = PI / 512.0f;

float theta = PI / 2.0f;
float thetaDiff = PI / 256.0f;

float radius = 10.0f;
float radiusDiff = 0.05f;

// Initial Field of View
float initialFoV = 45.0f;

bool lighting = true;
float speed = 0.01f;

// MODIFIED
void computeMatricesFromInputs() 
{
	/*
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
	*/
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		theta -= thetaDiff * deltaTime * speed;
		if (theta < 0.0f)
			theta = 0.001f;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		theta += thetaDiff * deltaTime * speed;
		if (theta > PI)
			theta = PI - 0.001f;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		phi -= phiDiff * deltaTime * speed;
		if (phi < 0.0f)
			phi = 2.0f * PI;
		//phi = (phi < 0.0f) ? (2.0f * PI) : (phi - phiDiff);
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		phi += phiDiff * deltaTime * speed;
		if (phi > 2.0f * PI)
			phi = 0.0f;
		//phi = (phi > 2.0f * PI) ? 0.0f : (phi + phiDiff);
	}
	// Zooming out
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		radius += radiusDiff * deltaTime * speed;
	}
	// Zooming in
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		radius -= radiusDiff * deltaTime * speed;
		if (radius < 0.0f)
			radius = 0.001f;
		//radius = (radius < 0.0f) ? 0.001f : (radius - radiusDiff);
	}
	

	float FoV = initialFoV;

	position.x = radius * sin(theta) * cos(phi);
	position.y = radius * sin(theta) * sin(phi);
	position.z = radius * cos(theta);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		origin,             // and looks here
		top                 // Head is up (set to 0, 0, 1 to look upside-down)
	);
}

// ADDED
void callbackForKeys(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		lighting = !lighting;
}
