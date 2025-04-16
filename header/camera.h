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
    float Speed, Sensitivity;
    float Yaw, Pitch;
    glm::vec3 Position{};
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f,  0.0f);

    Camera();

    explicit Camera(glm::vec3 position);

    Camera(glm::vec3 position, float speed, float sensitivity = 0.1f, float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 GetViewMatrix();

    void Move(Directions direction, float deltaTime);

    void ChangeDirection(float xOffset, float yOffset);
};



#endif //CAMERA_H
