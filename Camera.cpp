//
// Created by luky9 on 31.03.2025.
//

#include "Camera.h"

Camera::Camera() {
    Position = glm::vec3(0.0f, 0.0f, 0.0f);
    Speed = 2.5;
    Sensitivity = 0.1;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

Camera::Camera(glm::vec3 position) {
    Position = position;
    Speed = 2.5;
    Sensitivity = 0.1;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

Camera::Camera(glm::vec3 position, float speed, float sensitivity, float yaw, float pitch) {
    Position = position;
    Speed = speed;
    Sensitivity = sensitivity;
    Yaw = yaw;
    Pitch = pitch;
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::Move(Directions direction, float deltaTime) {
    float velocity = Speed * deltaTime;
    if (direction == FRONT)
        Position += Front * velocity;
    if (direction == BACK)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
    if (direction == RIGHT)
        Position += glm::normalize(glm::cross(Front, Up)) * velocity;
}

void Camera::ChangeDirection(float xOffset, float yOffset) {
    xOffset *= Sensitivity;
    yOffset *= Sensitivity;

    Yaw += xOffset;
    Pitch += yOffset;

    if(Pitch > 89.0f)
        Pitch =  89.0f;
    if(Pitch < -89.0f)
        Pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    direction.y = sin(glm::radians(Pitch));
    direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(direction);
    glm::vec3 right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(right, Front));
}


