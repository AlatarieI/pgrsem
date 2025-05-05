//
// Created by luky9 on 31.03.2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum Directions {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


class Camera {
public:
    float speed, sensitivity;
    float yaw, pitch;
    glm::vec3 position{};
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 sceneMin = glm::vec3(-50.0f, -10.0f, -50.0f);
    glm::vec3 sceneMax = glm::vec3(50.0f, 50.0f, 50.0f);
    bool shouldMove;

    Camera();

    explicit Camera(glm::vec3 position , bool shouldMove);

    Camera(glm::vec3 position, float speed, float sensitivity = 0.1f, float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix();

    void move(Directions direction, float deltaTime);

    void changeDirection(float xOffset, float yOffset);
};



#endif //CAMERA_H
