#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "glslObjects.h"
#include "shaderClass.h"
#include "cameraClass.h"
#include "model.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int createTexture(const std::string path);
unsigned int createCubemap(std::string base_path, std::vector<std::string> faces_paths);
void setUpVAO(unsigned int* VAO, std::vector<int> batches);
void setUpVBO(unsigned int* VBO, float* vertices, unsigned int sizeVertices);
void setUpEBO(unsigned int* EBO, unsigned int* indices, unsigned int sizeIndices);
void setUpFBOTexture(unsigned int* FBO, unsigned int* texture, unsigned int width, unsigned int height);
void setUpRBO(unsigned int* RBO, unsigned int width, unsigned int height);
void setUpFBOTextureAndRBO(unsigned int* FBO, unsigned int* texture, unsigned int* RBO, unsigned int width, unsigned int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool blinn = false;
bool blinnKeyPressed = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
#pragma region glfw: initialize and configure OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

#pragma endregion

#pragma region glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#pragma endregion

#pragma region glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#pragma endregion

#pragma region Configure global opengl state

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glEnable(GL_MULTISAMPLE);

#pragma endregion

#pragma region Build and compile shaders
    Shader shader("resources/shaders/ilumPlane/vsIlumPlane.vert", 
                  "resources/shaders/ilumPlane/fsIlumPlane.frag");
#pragma endregion

#pragma region Vertex data and set up VAOs and VBOs
    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

	VBO planeVBO(planeVertices, sizeof(planeVertices), {3, 3, 2}, 8);
	VAO planeVAO;
	planeVAO.addBuffer(planeVBO);

#pragma endregion

# pragma Load models

# pragma endregion

#pragma region Set up Uniform Buffer Object (UBO)

#pragma endregion

#pragma region Load textures, set them up as uniforms
	Texture textureWood("resources/textures/wood.png");

    shader.use();
	shader.setInt("texture_diffuse1", 0);

    
    shader.setVec3("pointLights[0].position", glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader.setVec3("pointLights[0].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setFloat("pointLights[0].constant", 1.0f);
    shader.setFloat("pointLights[0].linear", 0.09f);
    shader.setFloat("pointLights[0].quadratic", 0.032f);
#pragma endregion

#pragma region Other setup
#pragma endregion

#pragma region FPS counter

    float startTime = static_cast<float>(glfwGetTime());
    int frameCount = 0;

#pragma endregion

#pragma region render loop

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameCount++;
        if (currentFrame - startTime >= 1.0f) {
            startTime = currentFrame;
            std::cout << "\rFPS: " << frameCount << "" << std::flush;
            frameCount = 0;
        }

        // input
        // -----
        processInput(window);

        // Global variables
        // --------------------------------

        glm::mat4 view = camera.getLookAt();
        glm::mat4 projection = camera.getPerspective((float)SCR_WIDTH / (float)SCR_HEIGHT);
        glm::mat4 model = glm::mat4(1.0f);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Shader use
        // --------------------

        shader.use();
        planeVAO.bind();
        textureWood.bind(0);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("viewPos", camera.cameraPos);

        //  Plane
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
		shader.setMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
		shader.setBool("blinn", blinn);
        glDrawArrays(GL_TRIANGLES, 0, 6);
       

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#pragma endregion

#pragma region optional: de-allocate all resources
#pragma endregion
    planeVAO.del();
    planeVBO.del();
    glfwTerminate();
    return 0;
}

#pragma region Texture loading

unsigned int createCubemap(std::string base_path, std::vector<std::string> faces_paths) {

    unsigned int texture;
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.

    // texture upload and mipmaps
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    unsigned char* data;
    for (unsigned int i = 0; i < faces_paths.size(); i++)
    {
        data = stbi_load((base_path + faces_paths[i]).c_str(), &width, &height, &nrChannels, 0);

        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces_paths[i] << std::endl;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}

#pragma endregion

#pragma region Setup FBO and RBO

void setUpFBOTexture(unsigned int* FBO, unsigned int* texture, unsigned int width, unsigned int height) {
    glGenFramebuffers(1, FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
}

void setUpRBO(unsigned int* RBO, unsigned int width, unsigned int height) {
    glGenRenderbuffers(1, RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, *RBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *RBO); // now actually attach it
}

void setUpFBOTextureAndRBO(unsigned int* FBO, unsigned int* texture, unsigned int* RBO, unsigned int width, unsigned int height) {
    setUpFBOTexture(FBO, texture, width, height);
    setUpRBO(RBO, width, height);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

# pragma endregion

# pragma region Input callbacks

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
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
		cout << "\nBlinn-Phong: " << (blinn ? "ON" : "OFF") << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }
}

#pragma  endregion

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    SCR_HEIGHT = height;
    SCR_WIDTH = width;
}