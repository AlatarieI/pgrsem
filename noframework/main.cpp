#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char *VERTEX_FILE_NAME = "vertex_shader.vert";
const char *FRAGMENT_FILE_NAME = "fragment_shader.frag";

unsigned int program, VBO, VAO, EBO;
GLFWwindow* window;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void create_program() {
    FILE *vertexPtr, *fragmentPtr;

    vertexPtr = fopen(VERTEX_FILE_NAME, "r");
    if (vertexPtr == NULL) {
        perror("tf");
        std::cout << "Error opening file " << VERTEX_FILE_NAME << std::endl;
        exit(-1);
    }
    fseek(vertexPtr, 0, SEEK_END);
    int vertexSize = ftell(vertexPtr);
    fseek(vertexPtr, 0, SEEK_SET);

    fragmentPtr = fopen(FRAGMENT_FILE_NAME, "r");
    if (fragmentPtr == NULL) {
        std::cout << "Error opening file " << FRAGMENT_FILE_NAME << std::endl;
        exit(-1);
    }
    fseek(fragmentPtr, 0, SEEK_END);
    int fragmentSize = ftell(fragmentPtr);
    fseek(fragmentPtr, 0, SEEK_SET);

    char* vShaderCode = static_cast<char *>(malloc(vertexSize * sizeof(char)));
    char * fShaderCode = static_cast<char *>(malloc(fragmentSize * sizeof(char)));

    fread(vShaderCode,sizeof(char),vertexSize,vertexPtr);
    fread(fShaderCode,sizeof(char),fragmentSize,fragmentPtr);

    fclose(vertexPtr);
    fclose(fragmentPtr);

    int success;
    char infoLog[512];

    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Error: Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Error: Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // shader Program
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Error: Program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    free(vShaderCode);
    free(fShaderCode);
}

void set_up_buffers() {
    float vertices[] = {
        // positions         // colors
        0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // top left

        1.0f,  0.5f, 0.0f,  0.5f, 0.0f, 1.0f,    // right side
       -1.0f,  0.5f, 0.0f,  0.5f, 0.0f, 1.0f    // left side
   };
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3,    // second triangle

        1, 0, 4,    // right wing
        3, 2, 5    // left wing
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main() {
    init();
    create_program();
    set_up_buffers();

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}
