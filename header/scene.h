//
// Created by luky9 on 14.04.2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "lights.h"
#include "scene_object.h"

class Scene {
public:
    std::vector<std::string> modelPaths;
    std::vector<Model*> models;


    std::vector<SceneObject> objects;
    std::vector<DirectionalLight> dirLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;
    std::vector<Camera> cameras;
    int activeCameraIndex = 0;

    std::string skyTexturePath;

    int addModel(std::string path, bool flipUV = false, bool gamma = false);
    int addModel(Model *model);

    void addObject(std::string name, GLuint shader, int modelIndex, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    void addPointLight(PointLight light);
    void addDirLight(DirectionalLight light);
    void addSpotLight(SpotLight light);
    void addCamera(Camera camera);

    Camera* getActiveCamera();

    void load(const std::string& file);
    void save(const std::string& file);

    void draw(glm::mat4 projection);
};

#endif //SCENE_H
