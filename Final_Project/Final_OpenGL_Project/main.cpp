/*
Author: Shuojiang Liu
Class: ECE6122
Last Date Modified: Dec 6th, 2023
Description:
This is the main file of my ECE6122 final project.
In this project, I implemented a scene with 4 objects (2 cyborgs and 2 backpacks) and a floor.
The scene is in a hemispherical space, and the objects are moving randomly in the space.
The camera can be controlled by the user. The objects are illuminated by 4 point lights.
The objects are loaded from .obj files, and the floor is a simple rectangle.
The objects are rotating around their own axis, and the rotation axis is changing smoothly.
Also, they are moving randomly in the space, and the velocity is changing smoothly.
The objects will bounce back when they hit the hemispherical boundary, and will also bounce back when they hit each other.
The motion of the objects is calculated in a separate thread.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MyShader.h"
#include "MyCamera.h"
#include "model.h"

#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <random>

// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int loadTextureFromFile(const char* path);
bool handleCollision(glm::vec3& pos, glm::vec3& vel, float radius);
glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float factor);
void updateObjectPosition(glm::vec3& position, glm::vec3& velocity, float &angle, glm::vec3& axis, glm::vec3& targetAxis, float& lerpFactor);

void updateVelocity(glm::vec3& velocity);
bool checkCollision(const glm::vec3& pos1, const glm::vec3& pos2, float radius);

// =====================================================================================================================
// settings
const unsigned int SCR_WIDTH = 2000;  // screen width
const unsigned int SCR_HEIGHT = 1600;  // screen height

// number of objects
const unsigned int OBJECT_COUNT = 4;

// camera
MyCamera myCam(glm::vec3(0.0f, -3.0f, 0.0f), 5.0f, 0.0f, 0.0f );

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// we don't want the animation effect when first entering the scene
bool isAnimating = false;

// generate random number
std::random_device randomDevice;  // Will be used to obtain a seed for the random number engine
std::default_random_engine generator(randomDevice());  // Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

float sphereRadius = 4.0f; // Radius of the hemispherical space
float objectRadius = 1.0f;  // Raduis of the object

// =====================================================================================================================
std::vector<glm::vec3> velocities(4, glm::vec3(0.0f));  // initial velocities of the objects
std::mutex positionMutex;  // mutex for position update

// initialize the position of the objects
glm::vec3 modelPositions[OBJECT_COUNT] = {
        glm::vec3(-3.0f, -3.0f, 0.0f),
        glm::vec3(-1.0f, -2.0f, 0.0f),
        glm::vec3(1.0f, -3.0f, 0.0f),
        glm::vec3(3.0f, -2.0f, 0.0f)
};

// initialize the rotation of the objects
float rotationAngle[OBJECT_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f};

// initialize the rotation axis of the objects
glm::vec3 rotationAxis[OBJECT_COUNT] = {
    glm::normalize(glm::vec3(distribution(generator), distribution(generator), distribution(generator))),
    glm::normalize(glm::vec3(distribution(generator), distribution(generator), distribution(generator))),
    glm::normalize(glm::vec3(distribution(generator), distribution(generator), distribution(generator))),
    glm::normalize(glm::vec3(distribution(generator), distribution(generator), distribution(generator)))
};

// initialize the target rotation axis of the objects (for smooth rotation)
glm::vec3 targetRotationAxis[OBJECT_COUNT] = {
	glm::vec3(0.0f),
    glm::vec3(0.0f),
    glm::vec3(0.0f),
    glm::vec3(0.0f)
};

// initialize the lerp factor of the objects (for smooth rotation)
float lerpFactor[OBJECT_COUNT] = {0.1f, 0.1f, 0.1f, 0.1f};

// initialize the position of the point lights
glm::vec3 pointLightPositions[OBJECT_COUNT] = {
    modelPositions[0],
    modelPositions[1],
    modelPositions[2],
    modelPositions[3]
};

// our main function
int main()
{
    // =====================================================================================================================
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // I'm using Windows 11, so this is not needed (just keep it here for future reference)
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ECE6122 Final Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // =====================================================================================================================
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // =====================================================================================================================
    // build and compile shaders
    // -------------------------
    Shader ourShader("model_loading.vs", "model_loading.fs");
    Shader lightCubeShader("lighting_cube.vs", "lighting_cube.fs");
    Shader floorShader("floor.vs", "floor.fs");

    // =====================================================================================================================
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // position of floor
    float floorVertices[] = {
        // first triangle
         0.5f,  -0.4f, -0.5f, 1.0f, 1.0f, // top right
         0.5f, -0.4f, 0.5f, 1.0f, 0.0f, // bottom right
        -0.5f,  -0.4f, -0.5f, 0.0f, 1.0f,  // top left 
        // second triangle
         0.5f, -0.4f, 0.5f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.4f, 0.5f, 0.0f, 0.0f,  // bottom left
        -0.5f,  -0.4f, -0.5f, 0.0f, 1.0f   // top left
    };

    // =====================================================================================================================
    // configure the light's VAO and VBO
    unsigned int VBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

	// configure the floor's VAO and VBO
    unsigned int floorVBO, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glBindVertexArray(floorVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int floorTexture = loadTextureFromFile("./floor.jpg");  // load the texture

    // =====================================================================================================================
    // load 4 models
    // -----------
    Model ourModel("./cyborg/cyborg.obj");
    Model ourModelBackpack("./backpack/backpack.obj");
    Model ourModel2("./cyborg/cyborg.obj");
    Model ourModelBackpack2("./backpack/backpack.obj");

    // process the floor
    floorShader.use();
	floorShader.setInt("texture1", 0);

    // create a thread to update the position of the objects
	std::thread simulationThread([&]() {
        while (!glfwWindowShouldClose(window)) {
            for (unsigned int i = 0; i < OBJECT_COUNT; ++i) { // for each object
                updateObjectPosition(modelPositions[i], velocities[i], rotationAngle[i], rotationAxis[i], targetRotationAxis[i], lerpFactor[i]);
            }
            // sleep for 10 milliseconds to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::uniform_real_distribution<float> colorDistribution(0.2f, 1.0f); // random color for the point lights
    float targetDiffuseLight = colorDistribution(generator);  // target diffuse light (for smooth transition)
    float diffuseLight = colorDistribution(generator);  // current diffuse light
    float lightingLerpFactor = 0.0f;  // lerp factor for smooth transition

    // =====================================================================================================================
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =====================================================================================================================
        // enable shader before setting uniforms
        ourShader.use();

        ourShader.setVec3("viewPos", myCam.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable.
        */
        // ------------------------------
        // set the lighting color randomly
        if (lightingLerpFactor >= 1.0f)
        {
	        targetDiffuseLight = colorDistribution(generator);
            lightingLerpFactor = 0.0f;
        }
        else
        {
            diffuseLight = diffuseLight * (1.0f - lightingLerpFactor) + targetDiffuseLight;
            if (diffuseLight >= 1.0f) diffuseLight = 1.0f;
            lightingLerpFactor += deltaTime * 2.3f;
        }
        // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    	// point light 1
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", diffuseLight, diffuseLight, diffuseLight);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);

    	// point light 2
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[1].diffuse", diffuseLight, diffuseLight, diffuseLight);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09f);
        ourShader.setFloat("pointLights[1].quadratic", 0.032f);

    	// point light 3
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[2].diffuse", diffuseLight, diffuseLight, diffuseLight);
        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09f);
        ourShader.setFloat("pointLights[2].quadratic", 0.032f);

    	// point light 4
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[3].diffuse", diffuseLight, diffuseLight, diffuseLight);
        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09f);
        ourShader.setFloat("pointLights[3].quadratic", 0.032f);

    	// spotLight (we do not use it for this project)
        /*ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));*/


        // =====================================================================================================================
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = myCam.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        // -----------------------
        // object 1: cyborg
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, modelPositions[0]); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(rotationAngle[0]), rotationAxis[0]);
        glm::vec4 lightPosModelSpace = glm::vec4(0.0f, 1.5f, 0.0f, 1.0f);
        glm::vec4 lightPosWorldSpace = model * lightPosModelSpace;
        pointLightPositions[0] = glm::vec3(lightPosWorldSpace.x, lightPosWorldSpace.y, lightPosWorldSpace.z);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // object 2: backpack
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPositions[1]);
        model = glm::rotate(model, glm::radians(rotationAngle[1]), rotationAxis[1]);
        pointLightPositions[1] = modelPositions[1];
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        ourShader.setMat4("model", model);
        ourModelBackpack.Draw(ourShader);

        // object 3: cyborg 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPositions[2]);
        model = glm::rotate(model, glm::radians(rotationAngle[2]), rotationAxis[2]);
        lightPosWorldSpace = model * lightPosModelSpace;
        pointLightPositions[2] = glm::vec3(lightPosWorldSpace.x, lightPosWorldSpace.y, lightPosWorldSpace.z);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel2.Draw(ourShader);

        // object 4: backpack 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPositions[3]);
        model = glm::rotate(model, glm::radians(rotationAngle[3]), rotationAxis[3]);
        pointLightPositions[3] = modelPositions[3];
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        ourShader.setMat4("model", model);
        ourModelBackpack2.Draw(ourShader);

        // =====================================================================================================================
        // process light cubes

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        
        // we now draw as many light bulbs as we have point lights
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::rotate(model, glm::radians(rotationAngle[i]), rotationAxis[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("ourColor", glm::vec3(diffuseLight, diffuseLight, diffuseLight));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // =====================================================================================================================
        // Set the floor texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        floorShader.use();
        glBindVertexArray(floorVAO);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(15.0f));
        floorShader.setMat4("model", model);
        floorShader.setMat4("view", view);
        floorShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    	

        // =====================================================================================================================
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (simulationThread.joinable()) {
        simulationThread.join();
    }

    // ------------------------------------------------------------------
    // de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// =====================================================================================================================
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        isAnimating = true;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        myCam.ProcessKeyboard(PRESS_KEY_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        myCam.ProcessKeyboard(PRESS_KEY_RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        myCam.ProcessKeyboard(PRESS_KEY_UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        myCam.ProcessKeyboard(PRESS_KEY_DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        myCam.ProcessKeyboard(PRESS_KEY_U, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        myCam.ProcessKeyboard(PRESS_KEY_D, deltaTime);
}

// =====================================================================================================================
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// =====================================================================================================================
// utility function for loading a 2D texture from file
unsigned loadTextureFromFile(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// =====================================================================================================================
// update the velocity of the objects
void updateVelocity(glm::vec3& velocity) {
    // Randomly adjust velocity components
    velocity.x += distribution(generator) * 0.001f; // Scale to control speed
    velocity.y += distribution(generator) * 0.001f;
    velocity.z += distribution(generator) * 0.001f;
}

// check if there is a collision between two objects
bool checkCollision(const glm::vec3& pos1, const glm::vec3& pos2, float radius) {
    // Simple sphere-sphere collision detection
    return glm::distance(pos1, pos2) < (radius * 2);
}

// handle the collision between two objects
bool handleCollision(glm::vec3& pos, glm::vec3& vel, float radius) {
    bool collided = false;
    // Check collision with other objects
    for (unsigned int i = 0; i < OBJECT_COUNT; ++i) {
        if (&pos != &modelPositions[i]) {
            if (checkCollision(pos, modelPositions[i], radius)) {
                // vel = -vel; // Basic response - reverse velocity
                glm::vec3 direction = pos - modelPositions[i];
                direction = glm::normalize(direction);
                vel = direction * glm::length(vel); // Reflect velocity based on collision direction
                collided = true;
            }
        }
    }

    // Check if outside hemispherical boundary
    if (glm::length(pos) > sphereRadius) {
        // Reflect the object back into the hemisphere
        glm::vec3 toCenter = -pos; // Direction towards the center
        toCenter = glm::normalize(toCenter);
        // Adjust position and reverse velocity
        pos = glm::normalize(pos) * sphereRadius;
        // vel = -vel;
        vel = toCenter * glm::length(vel); // Reflect velocity towards the center
        collided = true;
    }

    return collided;
}

// linear interpolation function
glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float factor) {
    return glm::mix(start, end, factor);
}

// function to update the position of the objects
void updateObjectPosition(glm::vec3& position, glm::vec3& velocity, float &angle, glm::vec3& axis, glm::vec3& targetAxis, float& lerpFactor) {
    if (isAnimating) {
        // Lock the mutex to safely update the object's position
        std::lock_guard<std::mutex> lock(positionMutex);
        updateVelocity(velocity);
        position += velocity;

        if (lerpFactor < 1.0f) {
            axis = lerp(axis, targetAxis, lerpFactor);
            lerpFactor += 0.05f; // Adjust this value for interpolation speed
        }

        // Check for collision and handle it
        bool collisionDetected = handleCollision(position, velocity, objectRadius);

        angle += 0.5f; // we can adjust this value for rotation speed
        if (angle >= 360.0f) angle -= 360.0f; // Keep angle in range [0, 360)
        if (collisionDetected)
        {
            // Set new target rotation axis and reset lerp factor to 0
            targetAxis = lerp(axis, targetAxis, lerpFactor);
        	lerpFactor = 0.0f;
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

