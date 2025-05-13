//----------------------------------------------------------------------------------------
/**
 * \file       camera.h
 * \author     skorelu1
 * \date       2025/03/31
 * \brief      A class representing a camera in a 3D scene.
 *
 *  This class implements a first-person camera system with movement controls,
 *  view direction manipulation, and scene boundary constraints. It generates
 *  view matrices compatible with OpenGL and GLM mathematics.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

/// Enumeration for camera movement directions
enum Directions {
    FRONT,  ///< Move forward direction
    BACK,   ///< Move backward direction
    LEFT,   ///< Move left direction
    RIGHT,  ///< Move right direction
    UP,     ///< Move upward direction
    DOWN    ///< Move downward direction
};


/// Camera class for 3D scene navigation
/**
 * This class manages camera position, orientation, and movement constraints.
 * It provides functionality to generate view matrices and handle user input
 * for camera control.
 */
class Camera {
public:
    float speed;        ///< Movement speed in units per second
    float sensitivity;  ///< Mouse sensitivity for camera rotation
    float rotationY;          ///< Horizontal rotation angle in degrees
    float rotationX;        ///< Vertical rotation angle in degrees

    glm::vec3 position{};       ///< Camera position in world space
    glm::vec3 lastPosition{};   ///< Camera position in world space last frame
    glm::vec3 front;            ///< Calculated front direction vector
    glm::vec3 up;               ///< Camera's local up vector
    glm::vec3 worldUp;          ///< World's up vector reference
    glm::vec3 sceneMin;         ///< Minimum boundary for camera movement
    glm::vec3 sceneMax;         ///< Maximum boundary for camera movement
    bool shouldMove;            ///< Flag indicating if camera can move

    /// Default constructor
    Camera();

    /// Constructor with position and movement flag
    /**
     * \param[in] position   Initial camera position in world space
     * \param[in] shouldMove Initial movement state flag
     */
    explicit Camera(glm::vec3 position, bool shouldMove);

    /// Full parameter constructor
    /**
     * \param[in] position      Initial camera position in world space
     * \param[in] speed         Movement speed in units per second
     * \param[in] sensitivity   Mouse sensitivity for rotation (default: 0.1f)
     * \param[in] yaw           Initial horizontal rotation in degrees (default: -90.0f)
     * \param[in] pitch         Initial vertical rotation in degrees (default: 0.0f)
     */
    Camera(glm::vec3 position, float speed, float sensitivity = 0.1f, float yaw = -90.0f, float pitch = 0.0f);

    /// Get the view matrix for this camera
    /**
     * \return The computed view matrix using camera position and orientation
     */
    glm::mat4 getViewMatrix();

    /// Move the camera in specified direction
    /**
     * \param[in] direction   Movement direction from Directions enum
     * \param[in] deltaTime   Time since last frame for speed calculation
     */
    void move(Directions direction, float deltaTime);

    /// Update camera orientation based on mouse input
    /**
     * \param[in] xOffset   Mouse movement in X-axis (yaw rotation)
     * \param[in] yOffset   Mouse movement in Y-axis (pitch rotation)
     */
    void changeDirection(float xOffset, float yOffset);

    /// Calculates if camera collided with an object
    /**
     * \param[in] aabbMin   Minimum of AABB to check a collision with
     * \param[in] aabbMax   Maximum of AABB to check a collision with
     */
    bool intersectsAABB(glm::vec3 aabbMin, glm::vec3 aabbMax);
};

#endif // CAMERA_H