//
// Created by luky9 on 14.04.2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp> // Be sure to add nlohmann/json to your project
#include <iostream>

#include "camera.h"
#include "model.h"
#include "lights.h"
#include "scene_object.h"

class Scene {
public:
    // Shared resources
    std::vector<std::string> modelPaths;      // Paths to all models
    std::vector<Model*> models;               // Loaded models matching the paths


    std::vector<SceneObject> objects;
    std::vector<DirectionalLight> dirLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;
    std::vector<Camera> cameras;
    int activeCameraIndex = 0;

    // Sky dome
    std::string skyTexturePath;

    // Methods
    int addModel(std::string path);
    void addObject(int modelIndex, glm::vec3 position);
    void addObject(std::string path, glm::vec3 position);
    void addPointLight(PointLight light);
    void addDirLight(DirectionalLight light);
    void addSpotLight(SpotLight light);
    void addCamera(Camera camera);

    Camera* getActiveCamera();

    void load(const std::string& file);
    void save(const std::string& file);

    void draw();
};

#endif //SCENE_H
