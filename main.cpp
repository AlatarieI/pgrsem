#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fstream>
#include <sstream>
#include <iostream>

#include <camera.h>

#include <sky_box.h>

#include "model.h"
#include "scene.h"
#include "utility.h"
#include "data.h"

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

bool doPicking = false;
bool showUI = false;
int pickedObjectIdx = -1;
double mouseX, mouseY;

GLuint main_shader, light_cube_shader, VBO, VAO, EBO, texture1, texture2, light_cube_VAO;
GLFWwindow* window;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);
GLint modelLoc, viewLoc, projLoc, lightPosLoc, viewPosLoc;

GLint lightAmbientLoc, lightDiffuseLoc, lightSpecularLoc, materialAmbientLoc, materialDiffuseLoc, materialSpecularLoc, materialShininessLoc;

glm::vec3 lightPos = glm::vec3(0.0f, 4.0f, -8.0f);

Camera* currentCamera;
Camera camera1(glm::vec3(0.0f, 0.0f, 2.0f), true);
Camera camera2(glm::vec3(0.0f, 0.0f, 12.0f), false);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;

bool firstMouse = true, showCursor = false, isDragging = false;

Scene scene;


glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

std::unordered_map<int, bool> keyState;
std::unordered_map<int, bool> keyPressed;
std::unordered_map<int, bool> keyLastState;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    keyLastState[key] = keyState[key];
    if (action == GLFW_PRESS) {
        keyState[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyState[key] = false;
    }
}

void processInput(GLFWwindow *window) {
    currentCamera = scene.getActiveCamera();
    if (keyState[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    if (keyState[GLFW_KEY_LEFT_SHIFT])
        currentCamera->speed = 6.5f;
    else
        currentCamera->speed = 3.0f;

    if (keyState[GLFW_KEY_F1])
        scene.activeCameraIndex = 0;

    if (keyState[GLFW_KEY_F2])
        scene.activeCameraIndex = 1;

    if (keyPressed[GLFW_KEY_L])
        scene.cameraSpotlightActive = !scene.cameraSpotlightActive;

    if (keyState[GLFW_KEY_M]) {
        Camera* curr = scene.getActiveCamera();
        scene.activeCameraIndex = 2;
        Camera* moving = scene.getActiveCamera();

        moving->front = curr->front;
        moving->up = curr->up;
        moving->yaw = curr->yaw;
        moving->pitch = curr->pitch;
        moving->position = curr->position;

    }

    if (keyPressed[GLFW_KEY_F]) {
        scene.useFog = !scene.useFog;
    }


    if (keyState[GLFW_KEY_W])
        currentCamera->move(FRONT, deltaTime);
    if (keyState[GLFW_KEY_S])
        currentCamera->move(BACK, deltaTime);
    if (keyState[GLFW_KEY_A])
        currentCamera->move(LEFT, deltaTime);
    if (keyState[GLFW_KEY_D])
        currentCamera->move(RIGHT, deltaTime);
    if (keyState[GLFW_KEY_SPACE])
        currentCamera->move(UP, deltaTime);
    if (keyState[GLFW_KEY_LEFT_CONTROL])
        currentCamera->move(DOWN, deltaTime);

    if (keyState[GLFW_KEY_UP])
        currentCamera->changeDirection(0.0f, 1.0f);
    if (keyState[ GLFW_KEY_DOWN])
        currentCamera->changeDirection(0.0f, -1.0f);
    if (keyState[GLFW_KEY_LEFT])
        currentCamera->changeDirection(-1.0f, 0.0f);
    if (keyState[GLFW_KEY_RIGHT])
        currentCamera->changeDirection(1.0f, 0.0f);

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        showCursor = !showCursor;
        showUI = !showUI;
        if (!showCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(window, &mouseX, &mouseY);
            doPicking = true;
            isDragging = true;
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}


void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    currentCamera = scene.getActiveCamera();
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    if (!showCursor)
        currentCamera->changeDirection(xOffset, yOffset);
    else if (isDragging && !ImGui::GetIO().WantCaptureMouse)
        currentCamera->changeDirection(-xOffset, -yOffset);
}

void init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Nothing is fine", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Initialize backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330"); // Use appropriate GLSL version
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

void box_set_up() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}



void locations_setup() {
    glUseProgram(main_shader);
    modelLoc = glGetUniformLocation(main_shader, "model");
    viewLoc = glGetUniformLocation(main_shader, "view");
    projLoc = glGetUniformLocation(main_shader, "projection");

    materialAmbientLoc = glGetUniformLocation(main_shader, "material.ambient");
    materialDiffuseLoc = glGetUniformLocation(main_shader, "material.diffuse");
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


void setLightUniforms() {
    // Directional light
    glUniform3f(glGetUniformLocation(main_shader, "dirLight.direction"),
                                    glm::sin(glm::radians(-30.0f)) * glm::cos(glm::radians(70.0f)),
                                    glm::sin(glm::radians(-30.0f)) * glm::sin(glm::radians(70.0f)),
                                    glm::cos(glm::radians(-30.0f))); // approximate direction to sky dome sun
    glUniform3f(glGetUniformLocation(main_shader, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(main_shader, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(main_shader, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

    // Point lights
    for (int i = 0; i < 4; ++i) {
        std::string idx = "pointLights[" + std::to_string(i) + "]";
        glUniform3fv(glGetUniformLocation(main_shader, (idx + ".position").c_str()), 1, glm::value_ptr(pointLightPositions[i]));
        glUniform3f(glGetUniformLocation(main_shader, (idx + ".ambient").c_str()), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(main_shader, (idx + ".diffuse").c_str()), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(main_shader, (idx + ".specular").c_str()), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(main_shader, (idx + ".constant").c_str()), 1.0f);
        glUniform1f(glGetUniformLocation(main_shader, (idx + ".linear").c_str()), 0.09f);
        glUniform1f(glGetUniformLocation(main_shader, (idx + ".quadratic").c_str()), 0.032f);
    }

    // Spotlight
    glUniform3fv(glGetUniformLocation(main_shader, "spotLight.position"), 1, glm::value_ptr(currentCamera->position));
    glUniform3fv(glGetUniformLocation(main_shader, "spotLight.direction"), 1, glm::value_ptr(currentCamera->front));
    glUniform3f(glGetUniformLocation(main_shader, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(main_shader, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(main_shader, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(main_shader, "spotLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(main_shader, "spotLight.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(main_shader, "spotLight.quadratic"), 0.032f);
    glUniform1f(glGetUniformLocation(main_shader, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
    glUniform1f(glGetUniformLocation(main_shader, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
}

void renderUI(Scene& scene) {
    static char fileName[128] = "scene.json";

    if (!showUI) return;

    ImGui::Begin("Scene Editor");

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Scene")) {
            ImGui::InputText("File Name", fileName, 128);
            if (ImGui::Button("Save Scene")) scene.save(fileName);
            if (ImGui::Button("Load Scene")) {
                scene.load(fileName);
                pickedObjectIdx = -1;  // Clear selected
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Models")) {
            static char modelPath[256] = "";
            ImGui::InputText("Model Path", modelPath, 256);
            if (ImGui::Button("Add Model") && modelPath[0] != '\0') {
                scene.addModel(modelPath);
                modelPath[0] = '\0';
            }

            ImGui::SeparatorText("Loaded Models:");
            for (int i = 0; i < scene.modelPaths.size(); ++i) {
                ImGui::Text("%d: %s", i, scene.modelPaths[i].c_str());
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Objects")) {
            static char objectName[128] = "";
            static int shaderIndex = 0;
            static int modelIndex = 0;

            ImGui::InputText("Object Name", objectName, 128);
            ImGui::InputInt("Shader Index", &shaderIndex);
            ImGui::InputInt("Model Index", &modelIndex);
            if (ImGui::Button("Add Object")) {
                scene.addObject(objectName, shaderIndex, modelIndex, {0,0,0}, {0,0,0}, {1,1,1});
                objectName[0] = '\0';
            }

            ImGui::SeparatorText("Loaded Objects:");
            for (int i = 0; i < scene.objects.size(); ++i) {
                ImGui::Text("%d: %s", i, scene.objects[i].name.c_str());
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("SkyBox")) {
            SkyBox* dome = scene.skyBox;

            ImGui::DragFloat("Time", &scene.time, 0.01f,0.0f,1.0f );

            ImGui::SliderInt("Current texture", &dome->currTextureIdx, 0, dome->textures.size()-1);
            ImGui::Checkbox("Blend texture", &dome->shouldBlend);
            ImGui::DragFloat("Blend factor", &scene.blend, 0.01f,0.0f,1.0f );
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lights")) {
            if (ImGui::CollapsingHeader("Directional Lights")) {
                for (int i = 0; i < scene.dirLights.size(); ++i) {
                    ImGui::PushID(i);

                    auto& light = scene.dirLights[i];
                    if (ImGui::TreeNode(("Directional Light " + std::to_string(i)).c_str())) {
                        ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), 0.1f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));

                        if (ImGui::Button("Delete")) {
                            scene.dirLights.erase(scene.dirLights.begin() + i);
                            ImGui::TreePop();
                            ImGui::PopID();
                            break;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Point Lights")) {
                for (int i = 0; i < scene.pointLights.size(); ++i) {
                    ImGui::PushID(1000 + i);

                    auto& light = scene.pointLights[i];
                    if (ImGui::TreeNode(("Point Light " + std::to_string(i)).c_str())) {
                        ImGui::DragFloat3("Position", glm::value_ptr(light.position), 0.1f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));
                        ImGui::DragFloat("Constant", &light.constant, 0.01f, 0.0f, 10.0f);
                        ImGui::DragFloat("Linear", &light.linear, 0.01f, 0.0f, 1.0f);
                        ImGui::DragFloat("Quadratic", &light.quadratic, 0.01f, 0.0f, 1.0f);
                        ImGui::InputInt("Attached Object Index", &light.objectIdx);

                        if (ImGui::Button("Delete")) {
                            scene.pointLights.erase(scene.pointLights.begin() + i);
                            ImGui::TreePop();
                            ImGui::PopID();
                            break;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Spot Lights")) {
                for (int i = 0; i < scene.spotLights.size(); ++i) {
                    ImGui::PushID(2000 + i);

                    auto& light = scene.spotLights[i];
                    if (ImGui::TreeNode(("Spot Light " + std::to_string(i)).c_str())) {
                        ImGui::DragFloat3("Position", glm::value_ptr(light.position), 0.1f);
                        ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), 0.1f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));
                        ImGui::DragFloat("Cut Off", &light.cutOff, 0.01f, 0.0f, glm::pi<float>());
                        ImGui::DragFloat("Outer Cut Off", &light.outerCutOff, 0.01f, 0.0f, glm::pi<float>());
                        ImGui::DragFloat("Constant", &light.constant, 0.01f, 0.0f, 10.0f);
                        ImGui::DragFloat("Linear", &light.linear, 0.01f, 0.0f, 1.0f);
                        ImGui::DragFloat("Quadratic", &light.quadratic, 0.01f, 0.0f, 1.0f);
                        ImGui::InputInt("Attached Object Index", &light.objectIdx);

                        if (ImGui::Button("Delete")) {
                            scene.spotLights.erase(scene.spotLights.begin() + i);
                            ImGui::TreePop();
                            ImGui::PopID();
                            break;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Cameras")) {
            ImGui::Text("Camera management coming soon...");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Selected")) {
            if (pickedObjectIdx >= 0) {
                SceneObject& obj = scene.objects[pickedObjectIdx];

                char* nameBuffer = &obj.name[0];
                ImGui::InputText("Name", nameBuffer, obj.name.capacity() + 1);
                obj.name = nameBuffer;

                bool changed = false;
                changed |= ImGui::DragFloat3("Position", glm::value_ptr(obj.position), 0.1f);
                changed |= ImGui::DragFloat3("Rotation", glm::value_ptr(obj.rotation), 0.1f);
                changed |= ImGui::DragFloat3("Scale",    glm::value_ptr(obj.scale), 0.1f);
                if (changed) {
                    obj.isDirty = true;
                }

                ImGui::InputInt("Shader Index", &obj.shaderIdx);
                ImGui::InputInt("Model Index", &obj.modelIndex);

                ImGui::SeparatorText("Attached Point Lights");

                int lightIdx = 0;
                for (auto& light : scene.pointLights) {
                    if (light.objectIdx == pickedObjectIdx) {
                        std::string label = "PointLight " + std::to_string(lightIdx++);
                        if (ImGui::TreeNode(label.c_str())) {
                            ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
                            ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
                            ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));

                            ImGui::DragFloat("Constant", &light.constant, 0.01f);
                            ImGui::DragFloat("Linear", &light.linear, 0.01f);
                            ImGui::DragFloat("Quadratic", &light.quadratic, 0.01f);

                            if (ImGui::Button("Unlink")) {
                                light.objectIdx = -1;  // Detach from object
                            }

                            ImGui::TreePop();
                        }
                    }
                }

                ImGui::SeparatorText("Attached Spot Lights");

                lightIdx = 0;
                for (auto& light : scene.spotLights) {
                    if (light.objectIdx == pickedObjectIdx) {
                        std::string label = "SpotLight " + std::to_string(lightIdx++);
                        if (ImGui::TreeNode(label.c_str())) {
                            ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), 0.1f);

                            ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
                            ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
                            ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));

                            ImGui::DragFloat("Cut Off", &light.cutOff, 0.01f);
                            ImGui::DragFloat("Outer Cut Off", &light.outerCutOff, 0.01f);

                            ImGui::DragFloat("Constant", &light.constant, 0.01f);
                            ImGui::DragFloat("Linear", &light.linear, 0.01f);
                            ImGui::DragFloat("Quadratic", &light.quadratic, 0.01f);

                            if (ImGui::Button("Unlink")) {
                                light.objectIdx = -1;
                            }

                            ImGui::TreePop();
                        }
                    }
                }

                ImGui::SeparatorText("Add Light to Object");

                if (ImGui::Button("Add Point Light")) {
                    PointLight newLight{};
                    newLight.objectIdx = pickedObjectIdx;
                    newLight.ambient = glm::vec3(0.2f);
                    newLight.diffuse = glm::vec3(0.5f);
                    newLight.specular = glm::vec3(1.0f);
                    newLight.constant = 1.0f;
                    newLight.linear = 0.09f;
                    newLight.quadratic = 0.032f;
                    newLight.position = glm::vec3(0.0f);
                    scene.addPointLight(newLight);
                }

                if (ImGui::Button("Add Spot Light")) {
                    SpotLight newLight{};
                    newLight.objectIdx = pickedObjectIdx;
                    newLight.ambient = glm::vec3(0.1f);
                    newLight.diffuse = glm::vec3(0.8f);
                    newLight.specular = glm::vec3(1.0f);
                    newLight.cutOff = 12.5f;
                    newLight.outerCutOff = 15.0f;
                    newLight.constant = 1.0f;
                    newLight.linear = 0.09f;
                    newLight.quadratic = 0.032f;
                    newLight.direction = glm::vec3(1.0f, 0.0f, 0.0f);
                    newLight.position = glm::vec3(0.0f);
                    scene.addSpotLight(newLight);
                }

                if (ImGui::Button("Delete")) {
                    // Unlink any lights attached to this object before deletion
                    for (auto& light : scene.pointLights) {
                        if (light.objectIdx == pickedObjectIdx)
                            light.objectIdx = -1;
                    }
                    for (auto& light : scene.spotLights) {
                        if (light.objectIdx == pickedObjectIdx)
                            light.objectIdx = -1;
                    }

                    scene.objects.erase(scene.objects.begin() + pickedObjectIdx);
                    pickedObjectIdx = -1;
                }

            } else {
                ImGui::Text("Click object to select it.");
            }

            ImGui::EndTabItem();
        }


        ImGui::EndTabBar();
    }

    ImGui::End();
}



int main() {
    init();

    scene.load("test.json");

    while(!glfwWindowShouldClose(window)) {
        currentCamera = scene.getActiveCamera();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        for (auto& [key, isDown] : keyState) {
            keyPressed[key] = (isDown && !keyLastState[key]);
            keyLastState[key] = isDown;
        }

        processInput(window);

        glClearColor(0.6f, 0.7f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH)/ static_cast<float>(SCR_HEIGHT) , 0.1f, 100.0f);
        view = currentCamera->getViewMatrix();
        lightPos = glm::vec3(-0.2f, -1.0f, -0.3f);

        // Start the frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        renderUI(scene);

        scene.draw(projection, deltaTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        if (!ImGui::GetIO().WantCaptureMouse && doPicking) {
            int winX = showCursor ? static_cast<int>(mouseX) : SCR_WIDTH/2;
            int winY = showCursor ? SCR_HEIGHT - static_cast<int>(mouseY) - 1 : SCR_HEIGHT/2; // Flip Y

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            scene.drawPicking(projection);

            glFlush();

            unsigned char pixel[4];
            glReadPixels(winX, winY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

            pickedObjectIdx = static_cast<int>(pixel[0]) - 1;
            // printf("%d %d %d \n", objectId, pixel[1], pixel[2]);
            std::cout << "Clicked on object " << pickedObjectIdx <<"\n";
        }

        doPicking = false;

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &light_cube_VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(main_shader);
    glDeleteProgram(light_cube_shader);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwTerminate();
    return 0;
}
