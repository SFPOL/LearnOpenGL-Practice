#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float PI = 3.14159265358979323846f;

// shaders
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in float initial_angle;\n"
"out float initial_angle_out;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   initial_angle_out = initial_angle;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in float initial_angle_out;\n"
"uniform float offset;\n"
"void main()\n"
"{\n"
"   FragColor = vec4((sin(initial_angle_out + offset)*0.70710678118)/2+0.5, (cos(initial_angle_out + offset)*0.70710678118)/2+0.5, 0.5, 1.0);\n"
"}\0";

float vertices[] = {
    // position         //angle
     0.5f,  0.5f, 0.0f, PI * 0.25f, // top right
     0.5f, -0.5f, 0.0f, PI * 0.75f,  // bottom right
    -0.5f, -0.5f, 0.0f, PI * 1.25f,  // bottom left
    -0.5f,  0.5f, 0.0f, PI * 1.75f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();  // Initialize the GLFW library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Set the OpenGL version to 3.3 (3 MAJOR, 3 MINOR)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set the OpenGL profile to Core Profile

    // glfw window creation
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
    // this callback function executes

// glad: load all OpenGL function pointers
// ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // VERTEX SHADER
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // We check if any errors occurred during the compilation of the vertex shader
    int  successVS;
    char infoLogVS[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successVS);
    if (!successVS)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLogVS);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLogVS << std::endl;
    }

    // FRAGMENT SHADER
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // We check if any errors occurred during the compilation of the fragment shader
    int  successFS;
    char infoLogFS[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successFS);
    if (!successFS)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogFS);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLogFS << std::endl;
    }

    //LINK SHADER PROGRAM
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // We check if any errors occurred during the linking of the shader program
    int  successSP;
    char infoLogSP[512];
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &successSP);
    if (!successSP)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLogSP);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLogSP << std::endl;
    }
    // And we delete the shaders as we don't longer need them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // Generate a Vertex Buffer Object (VBO), an Element Buffer Object (EBO), and a Vertex Array Object (VAO) ,
    // and bind it to the GL_ARRAY_BUFFER target
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO); // Generate 1 buffer and store its ID in VBO
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &VAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    // position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attributes
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO 
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify 
    // this VAO, but this rarely happens. Modifying other VAOs requires a call to 
    // glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's 
    // not directly necessary.
    glBindVertexArray(0);

    // We do the same for the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	int angleOffsetLocation = glGetUniformLocation(shaderProgram, "offset");


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		float time = glfwGetTime();

        // draw our first triangle
        glUseProgram(shaderProgram);
		glUniform1f(angleOffsetLocation, time);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
