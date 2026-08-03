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

    #pragma endregion

    #pragma region Build and compile shaders
    Shader shader("resources/shaders/simpleModel/vsSimple.vert", 
                  "resources/shaders/simpleModel/fsSimple.frag");

    Shader shaderNormals("resources/shaders/viewNormals/vsViewNorm.vert",
                         "resources/shaders/viewNormals/fsViewNorm.frag",
                         "resources/shaders/viewNormals/gsViewNorm.geom");

    #pragma endregion

    #pragma region Vertex data and set up VAOs and VBOs

    #pragma endregion

    # pragma Load models
        
    # pragma endregion

    #pragma region Set up Uniform Buffer Object (UBO)

    #pragma endregion

    #pragma region Load textures, set them up as uniforms
    stbi_set_flip_vertically_on_load(true);
    Model backpack("C:/Users/sfpol/OneDrive/Programes/OpenGLProject/LearnOpenGL/LearnOpenGL/resources/models/backpack/backpack.obj");
    #pragma endregion
    
    #pragma region render loop

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


        // We first render the backpack
        // configure transformation matrices
		glm::mat4 projection = camera.getPerspective((float)SCR_WIDTH / (float)SCR_HEIGHT);
        glm::mat4 view = camera.getLookAt();
        glm::mat4 model = glm::mat4(1.0f);
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        // draw model
        backpack.Draw(shader);

		//// Now we render the normals of the backpack
		shaderNormals.use();
        shaderNormals.setMat4("projection", projection);
        shaderNormals.setMat4("view", view);
        shaderNormals.setMat4("model", model);
		shaderNormals.setMat3("normalMatrix", glm::mat3(view * glm::transpose(glm::inverse(model))));
		backpack.Draw(shaderNormals);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    #pragma endregion

    #pragma region optional: de-allocate all resources 

    #pragma endregion

    glfwTerminate();
    return 0;
}

#pragma region Texture loading

unsigned int createTexture(const std::string path) {

    unsigned int texture;
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // texture upload and mipmaps
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        if (format == GL_RGBA) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else {
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps

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

#pragma region Setup basic objects

void setUpVAO(unsigned int* VAO, vector<int> batches) {
    glGenVertexArrays(1, VAO);

    glBindVertexArray(*VAO);


    unsigned int total_sum = 0;
    for (unsigned int i = 0; i < batches.size(); i++) {
        total_sum += abs(batches[i]);
    }

    unsigned int sum = 0;
    unsigned int batch_num = 0;
    for (unsigned int i = 0; i < batches.size(); i++) {
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

void setUpVBO(unsigned int* VBO, float* vertices, unsigned int sizeVertices) {
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeVertices, vertices, GL_STATIC_DRAW);
}

void setUpEBO(unsigned int* EBO, unsigned int* indices, unsigned int sizeIndices) {
    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices, GL_STATIC_DRAW);
}

#pragma endregion

#pragma region Setup FBO and RBO

void setUpFBOTexture(unsigned int* FBO, unsigned int* texture, unsigned int width, unsigned int height) {
    glGenFramebuffers(1, FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height , 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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