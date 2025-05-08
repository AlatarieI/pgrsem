// //
// // Created by luky9 on 24.04.2025.
// //
//
// #ifndef LIGHT_H
// #define LIGHT_H
//
// #include <glad/glad.h>
// #include <glm.hpp>
// #include <gtc/type_ptr.hpp>
// #include <string>
//
// class DirectionalLight{
// public:
//     glm::vec3 ambient;
//     glm::vec3 diffuse;
//     glm::vec3 specular;
//
//     glm::vec3 direction;
//
//     void setUniforms(GLuint lightingShader);
// };
//
// class PointLight{
// public:
//     glm::vec3 ambient;
//     glm::vec3 diffuse;
//     glm::vec3 specular;
//
//     glm::vec3 position;
//
//     float constant;
//     float linear;
//     float quadratic;
//
//     int objectIdx;
//
//     void setUniforms(GLuint lightingShader, int index);
// };
//
// class SpotLight{
// public:
//     glm::vec3 ambient;
//     glm::vec3 diffuse;
//     glm::vec3 specular;
//
//     glm::vec3 position;
//     glm::vec3 direction;
//     float cutOff;
//     float outerCutOff;
//
//     float constant;
//     float linear;
//     float quadratic;
//
//     int objectIdx;
//
//     void setUniforms(GLuint lightingShader);
// };
//
//
//
//
// #endif //LIGHT_H


//----------------------------------------------------------------------------------------
/**
 * \file       light.h
 * \author     skorelu1
 * \date       2025/04/24
 * \brief      Lighting system classes for 3D rendering
 *
 *  Contains classes for different light types: directional lights, point lights,
 *  and spotlights. Handles light properties and shader uniform updates.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <string>

/// Directional light source (like sunlight)
/**
 * Simulates light coming from a constant direction across the entire scene
 */
class DirectionalLight {
public:
    glm::vec3 ambient;   ///< Ambient light intensity (RGB color)
    glm::vec3 diffuse;   ///< Diffuse light intensity (RGB color)
    glm::vec3 specular;  ///< Specular light intensity (RGB color)

    glm::vec3 direction; ///< Light direction vector (normalized)

    /// Upload light properties to shader
    /**
     * \param[in] lightingShader  OpenGL shader program ID for lighting calculations
     */
    void setUniforms(GLuint lightingShader);
};

/// Point light source (omnidirectional, with attenuation)
/**
 * Simulates light radiating from a specific point in all directions
 */
class PointLight {
public:
    glm::vec3 ambient;    ///< Ambient light intensity (RGB color)
    glm::vec3 diffuse;    ///< Diffuse light intensity (RGB color)
    glm::vec3 specular;   ///< Specular light intensity (RGB color)

    glm::vec3 position;   ///< World space position of the light

    float constant;  ///< Constant attenuation factor (a in a + b*d + c*d²)
    float linear;    ///< Linear attenuation factor (b in a + b*d + c*d²)
    float quadratic; ///< Quadratic attenuation factor (c in a + b*d + c*d²)

    int objectIdx;   ///< Associated 3D object index in the scene

    /// Upload light properties to shader array
    /**
     * \param[in] lightingShader  OpenGL shader program ID
     * \param[in] index           Index in shader's light array (e.g., pointLights[index])
     */
    void setUniforms(GLuint lightingShader, int index);
};

/// Spotlight source (directional cone with attenuation)
/**
 * Simulates focused light beam with inner and outer cone angles
 */
class SpotLight {
public:
    glm::vec3 ambient;     ///< Ambient light intensity (RGB color)
    glm::vec3 diffuse;     ///< Diffuse light intensity (RGB color)
    glm::vec3 specular;    ///< Specular light intensity (RGB color)

    glm::vec3 position;    ///< World space position of the light
    glm::vec3 direction;   ///< Light direction vector (normalized)
    float cutOff;          ///< Inner cutoff angle cosine (brightest area)
    float outerCutOff;     ///< Outer cutoff angle cosine (falloff area)

    float constant;   ///< Constant attenuation factor
    float linear;     ///< Linear attenuation factor
    float quadratic;  ///< Quadratic attenuation factor

    int objectIdx;    ///< Associated 3D object index in the scene

    /// Upload spotlight properties to shader
    /**
     * \param[in] lightingShader  OpenGL shader program ID
     */
    void setUniforms(GLuint lightingShader);
};

#endif // LIGHT_H