#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "shaderClass.h"
#include "cameraClass.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.h"

// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int createTexture(const std::string path);
void setUpVAO(unsigned int* VAO, int* batches, unsigned int numBatches);
void setUpVBO(unsigned int* VBO, float* vertices, unsigned int sizeVertices);
void setUpEBO(unsigned int* EBO, unsigned int* indices, unsigned int sizeIndices);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH/2 , lastY = SCR_HEIGHT/2;
bool firstMouse = true;

float mixValue = 0.2f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera;

float vertices2D[] = {
    // position         // color          //texture coords
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f   // top left 
};
unsigned int indices2D[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

float verticesCubeTexture[] = {
        // position           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

float verticesCube[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

float verticesCubeNormals[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

float verticesCubeNormalsTexture[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

int main()
{
#pragma region glfw: Initialize and configure
    glfwInit();  // Initialize the GLFW library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Set the OpenGL version to 3.3 (3 MAJOR, 3 MINOR)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set the OpenGL profile to Core Profile
#pragma endregion

#pragma region glfw: Creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the window's context current (so all changes to OpenGL state will affect this window)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // It says that whenever the window size changed (by OS or user resize) 
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    
#pragma endregion
     
#pragma region glad: We create Shaders and we set up the vertices/edges (VAO, VBO / EBO)
    Shader shaderCube("shaders/vShader.vs", "shaders/fShader.fs");
	Shader shaderLight("shaders/vShaderLight.vs", "shaders/fShaderLight.fs");

    unsigned int VAOLight, VBO;
    int batches[] = { 3 };
    int batchesLight[] = { 3 };
    setUpVBO(&VBO, verticesCube, sizeof(verticesCube));
    setUpVAO(&VAOLight, batchesLight, sizeof(batchesLight) / sizeof(batchesLight[0]));

    Model ourModel("C:/Users/sfpol/OneDrive/Programes/OpenGLProject/LearnOpenGL/LearnOpenGL/models/backpack/backpack.obj");
#pragma endregion

#pragma region Load, create and set texture 

#pragma endregion

#pragma region Matrices construction
    
    
#pragma endregion

#pragma region Other elements

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
#pragma endregion

#pragma region render loop
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // We paint the big Cube

            // We  activate the shader
        shaderCube.use();
        
            // We set the uniform variables

        //        // Material
        shaderCube.setVec3("viewPos", camera.cameraPos);
        shaderCube.setFloat("32", 32.0f);

                // Lights

        // directional light
        shaderCube.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        shaderCube.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shaderCube.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        shaderCube.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        // point light 1
        shaderCube.setVec3("pointLights[0].position", pointLightPositions[0]);
        shaderCube.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shaderCube.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shaderCube.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderCube.setFloat("pointLights[0].constant", 1.0f);
        shaderCube.setFloat("pointLights[0].linear", 0.09f);
        shaderCube.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        shaderCube.setVec3("pointLights[1].position", pointLightPositions[1]);
        shaderCube.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shaderCube.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shaderCube.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderCube.setFloat("pointLights[1].constant", 1.0f);
        shaderCube.setFloat("pointLights[1].linear", 0.09f);
        shaderCube.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        shaderCube.setVec3("pointLights[2].position", pointLightPositions[2]);
        shaderCube.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shaderCube.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shaderCube.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderCube.setFloat("pointLights[2].constant", 1.0f);
        shaderCube.setFloat("pointLights[2].linear", 0.09f);
        shaderCube.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        shaderCube.setVec3("pointLights[3].position", pointLightPositions[3]);
        shaderCube.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shaderCube.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        shaderCube.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderCube.setFloat("pointLights[3].constant", 1.0f);
        shaderCube.setFloat("pointLights[3].linear", 0.09f);
        shaderCube.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        shaderCube.setVec3("spotLight.position", camera.cameraPos);
        shaderCube.setVec3("spotLight.direction", camera.cameraDirection);
        shaderCube.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        shaderCube.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderCube.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderCube.setFloat("spotLight.constant", 1.0f);
        shaderCube.setFloat("spotLight.linear", 0.09f);
        shaderCube.setFloat("spotLight.quadratic", 0.032f);
        shaderCube.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shaderCube.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		

		        // Matrices
        glm::mat4 view = camera.getLookAt();
        glm::mat4 projection = camera.getPerspective((float)SCR_WIDTH / (float)SCR_HEIGHT);
        shaderCube.setMat4("view", view);
        shaderCube.setMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shaderCube.setMat4("model", model);
		shaderCube.setMat3("normalMatrix", glm::transpose(glm::inverse(model)));
        ourModel.Draw(shaderCube);

        // also draw the lamp object(s)
        shaderLight.use();
        shaderLight.setMat4("projection", projection);
        shaderLight.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(VAOLight);
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            shaderLight.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#pragma endregion

#pragma region de-allocate all resources
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAOLight);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
#pragma endregion

#pragma region glfw: terminate, clear all previously GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
#pragma endregion
}

unsigned int createTexture(const std::string path) {

    unsigned int texture;
    // ---------
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    GLenum format = GL_RGB;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

void setUpVAO(
    unsigned int* VAO,
    int* batches, unsigned int numBatches
) {
    glGenVertexArrays(1, VAO);

    glBindVertexArray(*VAO);

    unsigned int total_sum = 0;
    for (unsigned int i = 0; i < numBatches; i++) {
        total_sum += abs(batches[i]);
    }

    unsigned int sum = 0;
	unsigned int batch_num = 0;
    for (unsigned int i = 0; i < numBatches; i++) {
		if (batches[i] < 0) {
			sum -= batches[i];
		}
        else {
            glVertexAttribPointer(batch_num, batches[i], GL_FLOAT, GL_FALSE, total_sum * sizeof(float), (void*)(sum * sizeof(float)));
            glEnableVertexAttribArray(batch_num);
            sum += batches[i];
            batch_num++;
        }
        
    }
}

void setUpVBO(
	unsigned int* VBO,
	float* vertices, unsigned int sizeVertices
) {
	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeVertices, vertices, GL_STATIC_DRAW);
}

void setUpEBO(
    unsigned int* EBO,
    unsigned int* indices, unsigned int sizeIndices
) {
    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices, GL_STATIC_DRAW);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

	camera.moveDirection(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float sensitivity = 5.0f;
    camera.changeFOV(sensitivity * (float)yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
		camera.moveFrontBack(cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
		camera.moveFrontBack(-cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
		camera.moveLeftRight(cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.moveLeftRight(-cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
		camera.moveUpDown(cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.moveUpDown(-cameraSpeed);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
