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
    Shader asteroidShader("resources/shaders/asteroid/vsAst.vert",
        "resources/shaders/asteroid/fsAst.frag");

    Shader planetShader("resources/shaders/planet/vsPlt.vert",
        "resources/shaders/planet/fsPlt.frag");

    Shader skyboxShader("resources/shaders/cubemapShader/vsCubemap.vert",
        "resources/shaders/cubemapShader/fsCubemap.frag");
#pragma endregion

#pragma region Vertex data and set up VAOs and VBOs

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices), { 3 }, 3);
    VAO skyboxVAO;
    skyboxVAO.addBuffer(skyboxVBO);

    unsigned int amount = 1000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    glm::vec3* base_displacement;
    base_displacement = new glm::vec3[amount];
    float* base_scale;
    base_scale = new float[amount];
    float* base_rotAngle;
    base_rotAngle = new float[amount];
    glm::vec3* base_rot_axis;
    base_rot_axis = new glm::vec3[amount];
    srand(glfwGetTime()); // initialize random seed	
    float radius = 25.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = displacement;
        base_displacement[i] = glm::vec3(x, y, z);

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        base_scale[i] = scale;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        base_rotAngle[i] = rotAngle;
        base_rot_axis[i] = glm::vec3((rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f, (rand() % 1000) / 1000.0f);
        model = glm::rotate(model, rotAngle, base_rot_axis[i]);
    }

#pragma endregion

# pragma Load models
    Model rock("C:/Users/sfpol/OneDrive/Programes/OpenGLProject/LearnOpenGL/LearnOpenGL/resources/models/rock/rock.obj");
    Model planet("C:/Users/sfpol/OneDrive/Programes/OpenGLProject/LearnOpenGL/LearnOpenGL/resources/models/planet/planet.obj");
# pragma endregion

#pragma region Set up Uniform Buffer Object (UBO)

#pragma endregion

#pragma region Load textures, set them up as uniforms
    unsigned int skyboxTexture = createCubemap("resources/textures/space/", vector<std::string>({
        "right.png",
        "left.png",
        "top.png",
        "bottom.png",
        "front.png",
        "back.png"
        }));

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
#pragma endregion

#pragma region Other setup
    VBO buffer(modelMatrices, amount * sizeof(glm::mat4), { 4, 4, 4, 4 }, 16);

    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        VAO* VAO = rock.meshes[i].VAO;
        VAO->bind();
        // vertex attributes
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        VAO->unbind();
    }
#pragma endregion

    float startTime = static_cast<float>(glfwGetTime());
    int frameCount = 0;

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


        // 
        for (unsigned int i = 0; i < amount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float angle = (float)i / (float)amount * 360.0f + currentFrame / 10.0f;
            glm::vec3 displacement = base_displacement[i];
            displacement.x += sin(angle) * radius;
            displacement.z += cos(angle) * radius;
            model = glm::translate(glm::mat4(1.0f), displacement);

            // 2. scale: scale between 0.05 and 0.25f
            model = glm::scale(model, glm::vec3(base_scale[i]));

            // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
            float rotAngle = base_rotAngle[i] + currentFrame;
            model = glm::rotate(model, rotAngle, base_rot_axis[i]);

            // 4. now add to list of matrices
            modelMatrices[i] = model;
        }

        buffer.bind();
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            amount * sizeof(glm::mat4),
            modelMatrices
        );

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // configure transformation matrices
        glm::mat4 projection = camera.getPerspective((float)SCR_WIDTH / (float)SCR_HEIGHT);
        glm::mat4 view = camera.getLookAt();
        asteroidShader.use();
        asteroidShader.setMat4("projection", projection);
        asteroidShader.setMat4("view", view);
        asteroidShader.setVec3("viewPos", camera.cameraPos);
        asteroidShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -0.75f, -0.3f));
        asteroidShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        asteroidShader.setVec3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
        asteroidShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        planetShader.setVec3("viewPos", camera.cameraPos);
        planetShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -0.75f, -0.3f));
        planetShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        planetShader.setVec3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
        planetShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, currentFrame / 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        planetShader.setMat4("model", model);
        planet.Draw(planetShader);

        // draw meteorites
        asteroidShader.use();
        asteroidShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        for (unsigned int i = 0; i < rock.meshes.size(); i++)
        {
            rock.meshes[i].VAO->bind();
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            rock.meshes[i].VAO->unbind();
        }

        skyboxShader.use();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);
        skyboxVAO.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#pragma endregion

#pragma region optional: de-allocate all resources
#pragma endregion
    skyboxVAO.del();
    skyboxVBO.del();
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