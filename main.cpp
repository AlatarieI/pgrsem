#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

#include <Camera.h>


int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
const char *VERTEX_FILE_NAME = "vertex_shader.vert";
const char *FRAGMENT_FILE_NAME = "fragment_shader.frag";


GLuint main_shader, light_cube_shader, VBO, VAO, EBO, texture1, texture2, light_cube_VAO;
GLFWwindow* window;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);
GLint modelLoc, viewLoc, projLoc, lightPosLoc, viewPosLoc;

GLint lightAmbientLoc, lightDiffuseLoc, lightSpecularLoc, materialAmbientLoc, materialDiffuseLoc, materialSpecularLoc, materialShininessLoc;

glm::vec3 lightPos = glm::vec3(0.0f, 4.0f, -8.0f);

Camera MainCamera(glm::vec3(0.0f, 0.0f, 2.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;

bool firstMouse = true, move_camera = true;


glm::vec3 cubePosition = glm::vec3( 0.0f,  0.0f,  -2.0f);



void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        MainCamera.Speed = 6.5f;
    else
        MainCamera.Speed = 3.0f;

    if (move_camera) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            MainCamera.Move(FRONT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            MainCamera.Move(BACK, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            MainCamera.Move(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            MainCamera.Move(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            MainCamera.Move(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            MainCamera.Move(DOWN, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            MainCamera.ChangeDirection(0.0f, 0.3f);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            MainCamera.ChangeDirection(0.0f, -0.3f);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            MainCamera.ChangeDirection(-0.3f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            MainCamera.ChangeDirection(0.3f, 0.0f);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        move_camera = !move_camera;
        if (move_camera) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

}


void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    if (move_camera) {
        MainCamera.ChangeDirection(xOffset, yOffset);
    }
}

void init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Nothing is fine", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
}

std::string readFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


GLuint create_program(const char* vertex_file_name, const char* fragment_file_name) {
    std::string vertexCode = readFile(vertex_file_name);
    std::string fragmentCode = readFile(fragment_file_name);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    int success;
    char infoLog[512];

    GLuint vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "Error: Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "Error: Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // shader Program
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vertex);
    glAttachShader(prog, fragment);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cout << "Error: Program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return prog;
}

void buffers_set_up() {
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
    // first, configure the cube's VAO (and VBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    glGenVertexArrays(1, &light_cube_VAO);
    glBindVertexArray(light_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

GLuint load_texture(char const * path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
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
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void locations_setup() {
    glUseProgram(main_shader);
    modelLoc = glGetUniformLocation(main_shader, "model");
    viewLoc = glGetUniformLocation(main_shader, "view");
    projLoc = glGetUniformLocation(main_shader, "projection");

    lightPosLoc = glGetUniformLocation(main_shader, "light.position");
    lightAmbientLoc = glGetUniformLocation(main_shader, "light.ambient");
    lightDiffuseLoc = glGetUniformLocation(main_shader, "light.diffuse");
    lightSpecularLoc = glGetUniformLocation(main_shader, "light.specular");

    // materialAmbientLoc = glGetUniformLocation(main_shader, "material.ambient");
    // materialDiffuseLoc = glGetUniformLocation(main_shader, "material.diffuse");
    materialSpecularLoc = glGetUniformLocation(main_shader, "material.specular");
    materialShininessLoc = glGetUniformLocation(main_shader, "material.shininess");

    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUseProgram(light_cube_shader);
    GLint light_cube_modelLoc = glGetUniformLocation(light_cube_shader, "model");
    GLint light_cube_projLoc = glGetUniformLocation(light_cube_shader, "projection");

    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    glUniformMatrix4fv(light_cube_modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix4fv(light_cube_projLoc, 1, GL_FALSE, glm::value_ptr(projection));

}


int main() {
    init();
    main_shader = create_program("shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    light_cube_shader = create_program("shaders/lightCube.vert", "shaders/lightCube.frag");
    buffers_set_up();
    //texture_load_setup();
    GLuint diffuseMap = load_texture("resources/textures/container2.png");
    glUseProgram(main_shader);
    glUniform1i(glGetUniformLocation(main_shader, "material.diffuse"), 0);
    locations_setup();

    view = MainCamera.GetViewMatrix();

    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH)/ static_cast<float>(SCR_HEIGHT) , 0.1f, 100.0f);
        view = MainCamera.GetViewMatrix();
        lightPos = glm::vec3(0.0f, 4*cos(currentFrame), 4*sin(currentFrame));

        glUseProgram(main_shader);

        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glUniform3fv(lightPosLoc, 1, glm::value_ptr(view*glm::vec4(lightPos,1.0f)));
        glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(glm::vec3( 0.2f, 0.2f, 0.2f)));
        glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5)));
        glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

        // glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(glm::vec3( 1.0f, 0.5f, 0.31f)));
        // glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
        glUniform1f(materialShininessLoc, 32.0f);

        glBindVertexArray(VAO);

        model = glm::mat4(1.0f);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUseProgram(light_cube_shader);

        GLint light_cube_projLoc = glGetUniformLocation(light_cube_shader, "projection");
        GLint light_cube_viewLoc = glGetUniformLocation(light_cube_shader, "view");
        GLint light_cube_modelLoc = glGetUniformLocation(light_cube_shader, "model");

        glUniformMatrix4fv(light_cube_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(light_cube_projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(light_cube_modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(light_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &light_cube_VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(main_shader);
    glDeleteProgram(light_cube_shader);

    glfwTerminate();
    return 0;
}
