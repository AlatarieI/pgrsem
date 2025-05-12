//----------------------------------------------------------------------------------------
/**
 * \file       scene.h
 * \author     skorelu1
 * \date       2025/04/14
 * \brief      Main scene container and manager class
 *
 *  Manages all rendering entities, lighting, cameras and scene hierarchy.
 *  Handles loading/saving from JSON, object picking, and rendering.
 *  Maintains relationships between models, shaders and scene objects.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sky_box.h>
#include <gtc/type_ptr.hpp>
#include "camera.h"
#include "model.h"
#include "lights.h"
#include "scene_object.h"
#include "shader.h"
#include <regex>
#include <GLFW/glfw3.h>

/// Main scene management class
class Scene {
public:
    std::vector<std::string> modelPaths;  ///< Paths to model files for reloading
    std::vector<Model*> models;           ///< Loaded 3D models repository

    std::vector<Shader> shaders;          ///< Shader programs available in the scene
    std::vector<SceneObject> objects;     ///< All scene objects in hierarchy
    std::vector<DirectionalLight> dirLights;  ///< Directional light sources
    std::vector<PointLight> pointLights;  ///< Point light sources
    std::vector<SpotLight> spotLights;    ///< Spotlight sources
    std::vector<Camera> cameras;          ///< Available camera viewpoints

    int activeCameraIndex = 0;    ///< Currently active camera index
    int objectCameraIndex = 3;    ///< Camera index for object-focused view
    int curveCameraIndex = 4;     ///< Camera index for spline-based movement

    float time;                   ///< Global scene time for animations

    int pickingShaderIdx = 2;     ///< Shader index for object selection rendering
    float blend = 0.5f;           ///< Blending factor for transitions/effects

    SkyBox* skyBox;               ///< Optional skybox reference
    Texture fogTexture;           ///< Texture for fog density calculations
    bool useFog = false;          ///< Fog effect toggle
    bool cameraSpotlightActive;   ///< Follow-camera spotlight toggle

    /// Add shader from source strings
    /**
     * \param[in] vertexShaderSource    GLSL vertex shader code
     * \param[in] fragmentShaderSource  GLSL fragment shader code
     * \return Index of the new shader in shaders vector
     */
    int addShader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);

    /// Add precompiled shader
    /**
     * \param[in] shader  Preconfigured Shader object
     * \return Index of the added shader
     */
    int addShader(Shader shader);

    /// Load model from file
    /**
     * \param[in] path      Path to model file
     * \param[in] flipUV    Flip texture coordinates vertically
     * \param[in] gamma     Enable gamma correction
     * \return Index of the new model in models vector
     */
    int addModel(std::string path, bool flipUV = false, bool gamma = false);

    /// Add existing model
    /**
     * \param[in] model  Preloaded Model object pointer
     * \return Index of the added model
     */
    int addModel(Model *model);

    /// Create new scene object
    /**
     * \param[in] name          Object identifier
     * \param[in] shaderIndex   Shader program index
     * \param[in] modelIndex    Model geometry index
     * \param[in] position      Initial position
     * \param[in] rotation      Initial rotation (degrees)
     * \param[in] scale         Initial scale factors
     */
    void addObject(std::string name, int shaderIndex, int modelIndex,
                  glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    /// Add point light source
    void addPointLight(PointLight light);

    /// Add directional light source
    void addDirLight(DirectionalLight light);

    /// Add spotlight source
    void addSpotLight(SpotLight light);

    /// Add camera viewpoint
    void addCamera(Camera camera);

    /// Update all light uniforms in shader
    /**
     * \param[in] shader  OpenGL shader program ID
     */
    void setLightUniforms(GLuint shader);

    /// Update object-specific uniforms
    /**
     * \param[in] shader  OpenGL shader program ID
     * \param[in] obj     SceneObject to process
     */
    void setObjectUniforms(GLuint shader, SceneObject& obj);

    /// Get currently active camera
    /**
     * \return Reference to active Camera object
     */
    Camera& getActiveCamera();

    /// Load scene from JSON file
    /**
     * \param[in] file  Path to scene configuration file
     */
    void load(const std::string& file);

    /// Save scene to JSON file
    /**
     * \param[in] file  Output file path
     */
    void save(const std::string& file);

    /// Main rendering method
    /**
     * \param[in] projection  Camera projection matrix
     * \param[in] delta       Time since last frame (seconds)
     */
    void draw(glm::mat4 projection, float delta);

    /// Object selection rendering pass
    /**
     * \param[in] projection  Camera projection matrix
     */
    void drawPicking(glm::mat4 projection);
};

#endif // SCENE_H