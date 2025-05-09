//
// Created by luky9 on 31.03.2025.
//

#include "camera.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f,  0.0f);
    worldUp = glm::vec3(0.0f, 1.0f,  0.0f);
    sceneMin = glm::vec3(-50.0f, -10.0f, -50.0f);
    sceneMax = glm::vec3(50.0f, 50.0f, 50.0f);
    speed = 2.5;
    sensitivity = 0.1;
    rotationY = -90.0f;
    rotationX = 0.0f;
}

Camera::Camera(glm::vec3 position, bool shouldMove) {
    this->shouldMove = shouldMove;
    this->position = position;
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f,  0.0f);
    worldUp = glm::vec3(0.0f, 1.0f,  0.0f);
    sceneMin = glm::vec3(-50.0f, -10.0f, -50.0f);
    sceneMax = glm::vec3(50.0f, 50.0f, 50.0f);
    speed = 2.5;
    sensitivity = 0.1;
    rotationY = -90.0f;
    rotationX = 0.0f;
}

Camera::Camera(glm::vec3 position, float speed, float sensitivity, float yaw, float pitch) {
    this->position = position;
    this->speed = speed;
    this->sensitivity = sensitivity;
    this->rotationY = yaw;
    this->rotationX = pitch;
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f,  0.0f);
    worldUp = glm::vec3(0.0f, 1.0f,  0.0f);
    sceneMin = glm::vec3(-50.0f, -10.0f, -50.0f);
    sceneMax = glm::vec3(50.0f, 50.0f, 50.0f);
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::move(Directions direction, float deltaTime) {
    if (shouldMove) {
        float velocity = speed * deltaTime;
        if (direction == FRONT)
            position += front * velocity;
        if (direction == BACK)
            position -= front * velocity;
        if (direction == LEFT)
            position -= glm::normalize(glm::cross(front, up)) * velocity;
        if (direction == RIGHT)
            position += glm::normalize(glm::cross(front, up)) * velocity;
        if (direction == UP)
            position += worldUp * velocity;
        if (direction == DOWN)
            position -= worldUp * velocity;
    }

    position = glm::clamp(position, sceneMin, sceneMax);
}

void Camera::changeDirection(float xOffset, float yOffset) {
    if (shouldMove) {
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        rotationY += xOffset;
        rotationX += yOffset;

        if(rotationX > 89.0f)
            rotationX =  89.0f;
        if(rotationX < -89.0f)
            rotationX = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(rotationY)) * cos(glm::radians(rotationX));
        direction.y = sin(glm::radians(rotationX));
        direction.z = sin(glm::radians(rotationY)) * cos(glm::radians(rotationX));

        front = glm::normalize(direction);
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
}


