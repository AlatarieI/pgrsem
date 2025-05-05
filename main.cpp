#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

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

GLuint main_shader, VBO, VAO, EBO;
GLFWwindow* window;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;

bool firstMouse = true, showCursor = false, isDragging = false;

Scene scene;

glm::quat fragmentCurrentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::vec3 fragmentPos = glm::vec3(1.0f, 10.0f, 0.0f);
glm::vec3 fragmentLastForward =  glm::vec3(1.0f, 0.0f, 0.0f);


std::vector<glm::vec3> points = {
    {  5.0f,  0.0f,  0.0f },  // p0
    {  5.0f,  3.0f,  3.0f },  // p1
    {  3.0f,  4.0f,  5.0f },  // p2
    {  0.0f,  2.0f,  5.0f },  // p3

    { -3.0f,  0.0f,  5.0f },  // p4 = 2*p3 - p2
    { -5.0f, -1.0f,  3.0f },  // p5
    { -5.0f, -3.0f,  0.0f },  // p6

    { -5.0f, -5.0f, -3.0f },  // p7
    { -3.0f, -6.0f, -5.0f },  // p8
    {  0.0f, -4.0f, -5.0f },  // p9

    {  3.0f, -2.0f, -5.0f },  // p10 = 2*p9 - p8
    {  5.0f, -3.0f, -3.0f },  // p11 = 2*p0 - p1
    {  5.0f,  0.0f,  0.0f },  // p12 = p0
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
    Camera& currentCamera = scene.getActiveCamera();
    if (keyState[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    if (keyState[GLFW_KEY_LEFT_SHIFT])
        currentCamera.speed = 6.5f;
    else
        currentCamera.speed = 3.0f;

    if (keyState[GLFW_KEY_F1])
        scene.activeCameraIndex = 0;

    if (keyState[GLFW_KEY_F2])
        scene.activeCameraIndex = 1;

    if (keyPressed[GLFW_KEY_L])
        scene.cameraSpotlightActive = !scene.cameraSpotlightActive;

    if (keyState[GLFW_KEY_M]) {
        Camera curr = scene.getActiveCamera();
        scene.activeCameraIndex = 2;
        Camera moving = scene.getActiveCamera();

        moving.front = curr.front;
        moving.up = curr.up;
        moving.yaw = curr.yaw;
        moving.pitch = curr.pitch;
        moving.position = curr.position;

    }

    if (keyPressed[GLFW_KEY_F]) {
        scene.useFog = !scene.useFog;
    }


    if (keyState[GLFW_KEY_W])
        currentCamera.move(FRONT, deltaTime);
    if (keyState[GLFW_KEY_S])
        currentCamera.move(BACK, deltaTime);
    if (keyState[GLFW_KEY_A])
        currentCamera.move(LEFT, deltaTime);
    if (keyState[GLFW_KEY_D])
        currentCamera.move(RIGHT, deltaTime);
    if (keyState[GLFW_KEY_SPACE])
        currentCamera.move(UP, deltaTime);
    if (keyState[GLFW_KEY_LEFT_CONTROL])
        currentCamera.move(DOWN, deltaTime);

    if (keyState[GLFW_KEY_UP])
        currentCamera.changeDirection(0.0f, 1.0f);
    if (keyState[ GLFW_KEY_DOWN])
        currentCamera.changeDirection(0.0f, -1.0f);
    if (keyState[GLFW_KEY_LEFT])
        currentCamera.changeDirection(-1.0f, 0.0f);
    if (keyState[GLFW_KEY_RIGHT])
        currentCamera.changeDirection(1.0f, 0.0f);

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
    Camera& currentCamera = scene.getActiveCamera();
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
        currentCamera.changeDirection(xOffset, yOffset);
    else if (isDragging && !ImGui::GetIO().WantCaptureMouse)
        currentCamera.changeDirection(-xOffset, -yOffset);
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

void ivory_fragment_set_up() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, cube_n_vertices * cube_n_attribs_per_vertex * sizeof(float), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_n_triangles * 3 * sizeof(unsigned int), cube_triangles, GL_STATIC_DRAW);

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cube_n_attribs_per_vertex * sizeof(float), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cube_n_attribs_per_vertex * sizeof(float), (void*)(3*sizeof(float)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, cube_n_attribs_per_vertex * sizeof(float), (void*)(6*sizeof(float)));
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

glm::vec3 bezier(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    float u = 1.0f - t;
    return u*u*u*p0 + 3*u*u*t*p1 + 3*u*t*t*p2 + t*t*t*p3;
}

int main() {
    init();

    scene.load("test.json");
    main_shader = scene.shaders[0].id;

    ivory_fragment_set_up();

    while(!glfwWindowShouldClose(window)) {
        Camera& currentCamera = scene.getActiveCamera();
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
        view = currentCamera.getViewMatrix();

        // Start the frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        renderUI(scene);

        int numSegments = (points.size() - 1) / 3;
        float tTotal = fmod(glfwGetTime() * 0.05f, 1.0f); // slower loop
        float tPerSegment = 1.0f / numSegments;

        int currentSegment = (int)(tTotal / tPerSegment);
        float localT = (tTotal - currentSegment * tPerSegment) / tPerSegment;

        int i = currentSegment * 3;
        glm::vec3 p0 = points[i];
        glm::vec3 p1 = points[i + 1];
        glm::vec3 p2 = points[i + 2];
        glm::vec3 p3 = points[i + 3];

        glm::vec3 next = bezier(localT, p0, p1, p2, p3);
        glm::vec3 nextNext = bezier(localT + 0.01f, p0, p1, p2, p3);

        glm::vec3 forward = glm::normalize(next - nextNext);
        glm::quat rotationDelta = glm::rotation(fragmentLastForward, forward);

        fragmentCurrentRotation = rotationDelta * fragmentCurrentRotation;

        fragmentPos = next + glm::vec3(0.0f, 10.0f, 0.0f);
        fragmentLastForward = forward;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), fragmentPos) * glm::mat4_cast(fragmentCurrentRotation);
        // glm::mat4 model = glm::translate(glm::mat4(1.0f), fragmentPos);

        glUseProgram(main_shader);

        glBindVertexArray(VAO);

        glUniform3fv(glGetUniformLocation(main_shader, "material.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 0.95, 0.85)));
        glUniform3fv(glGetUniformLocation(main_shader, "material.specular"), 1, glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
        glUniform1f(glGetUniformLocation(main_shader, "material.shininess"), 32.0f);

        glUniform1i(glGetUniformLocation(main_shader, "useDiffuseTexture"), false);
        glUniform1i(glGetUniformLocation(main_shader, "useSpecularTexture"), false);

        scene.setLightUniforms(main_shader);

        glUniform3fv(glGetUniformLocation(main_shader, "viewPos"), 1, glm::value_ptr(currentCamera.position));
        glUniformMatrix4fv(glGetUniformLocation(main_shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(main_shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(main_shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // glDrawArrays(GL_TRIANGLES, 0, 36);
        glDrawElements(GL_TRIANGLES, cube_n_triangles * 3 , GL_UNSIGNED_INT, nullptr);

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
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(main_shader);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwTerminate();
    return 0;
}
