#include "scene.h"

using json = nlohmann::json;

int Scene::addModel(std::string path) {
    // Check if model already loaded
    for (int i = 0; i < modelPaths.size(); ++i) {
        if (modelPaths[i] == path)
            return i;
    }

    Model* newModel = new Model(path);
    models.push_back(newModel);
    modelPaths.push_back(path);
    return models.size() - 1;
}

void Scene::addObject(int modelIndex, glm::vec3 position) {
    if (modelIndex < 0 || modelIndex >= models.size()) return;
    SceneObject obj;
    obj.modelIndex = modelIndex;
    obj.position = position;
    objects.push_back(obj);
}

void Scene::addObject(std::string path, glm::vec3 position) {
    int index = addModel(path);
    addObject(index, position);
}

void Scene::addPointLight(PointLight light) {
    pointLights.push_back(light);
}

void Scene::addDirLight(DirectionalLight light) {
    dirLights.push_back(light);
}

void Scene::addSpotLight(SpotLight light) {
    spotLights.push_back(light);
}

void Scene::addCamera(Camera camera) {
    cameras.push_back(camera);
}

Camera* Scene::getActiveCamera() {
    if (activeCameraIndex >= 0 && activeCameraIndex < cameras.size())
        return &cameras[activeCameraIndex];
    return nullptr;
}

void Scene::draw() {
    for (const auto& obj : objects) {
        if (obj.modelIndex >= 0 && obj.modelIndex < models.size()) {
            // Set model matrix and pass to shader in actual usage
            models[obj.modelIndex]->draw(obj.shader); // Replace 0 with actual shader program
        }
    }
}

void Scene::save(const std::string& file) {
    json j;
    j["models"] = modelPaths;
    j["skyTexturePath"] = skyTexturePath;

    for (const auto& obj : objects) {
        j["objects"].push_back({
            {"modelIndex", obj.modelIndex},
            {"position", {obj.position.x, obj.position.y, obj.position.z}}
        });
    }

    for (const auto& light : pointLights) {
        j["pointLights"].push_back({
            {"position", {light.position.x, light.position.y, light.position.z}},
            {"ambient", {light.ambient.r, light.ambient.g, light.ambient.b}},
            {"diffuse", {light.diffuse.r, light.diffuse.g, light.diffuse.b}},
            {"specular", {light.specular.r, light.specular.g, light.specular.b}},
            {"constant", light.constant},
            {"linear", light.linear},
            {"quadratic", light.quadratic}
        });
    }

    for (const auto& light : dirLights) {
        j["directionalLights"].push_back({
            {"direction", {light.direction.x, light.direction.y, light.direction.z}},
            {"ambient", {light.ambient.r, light.ambient.g, light.ambient.b}},
            {"diffuse", {light.diffuse.r, light.diffuse.g, light.diffuse.b}},
            {"specular", {light.specular.r, light.specular.g, light.specular.b}}
        });
    }

    for (const auto& light : spotLights) {
        j["spotLights"].push_back({
            {"position", {light.position.x, light.position.y, light.position.z}},
            {"direction", {light.direction.x, light.direction.y, light.direction.z}},
            {"ambient", {light.ambient.r, light.ambient.g, light.ambient.b}},
            {"diffuse", {light.diffuse.r, light.diffuse.g, light.diffuse.b}},
            {"specular", {light.specular.r, light.specular.g, light.specular.b}},
            {"cutOff", light.cutOff},
            {"outerCutOff", light.outerCutOff},
            {"constant", light.constant},
            {"linear", light.linear},
            {"quadratic", light.quadratic}
        });
    }

    for (const auto& cam : cameras) {
        j["cameras"].push_back({
            {"position", {cam.Position.x, cam.Position.y, cam.Position.z}},
            {"front", {cam.Front.x, cam.Front.y, cam.Front.z}},
            {"up", {cam.Up.x, cam.Up.y, cam.Up.z}},
            {"yaw", cam.Yaw},
            {"pitch", cam.Pitch},
            {"speed", cam.Speed},
            {"sensitivity", cam.Sensitivity}
        });
    }

    j["activeCameraIndex"] = activeCameraIndex;

    std::ofstream out(file);
    if (!out) {
        std::cerr << "Failed to open file for saving: " << file << std::endl;
        return;
    }
    out << j.dump(4);
    out.close();
}




void Scene::load(const std::string& file) {
    std::ifstream in(file);
    if (!in) {
        std::cerr << "Failed to open file for loading: " << file << std::endl;
        return;
    }

    json j;
    in >> j;

    modelPaths.clear();
    models.clear();
    objects.clear();
    pointLights.clear();
    dirLights.clear();
    spotLights.clear();
    cameras.clear();

    for (const auto& path : j["models"]) {
        addModel(path);
    }

    if (j.contains("skyTexturePath")) {
        skyTexturePath = j["skyTexturePath"];
    }

    for (const auto& o : j["objects"]) {
        SceneObject obj;
        obj.modelIndex = o["modelIndex"];
        auto pos = o["position"];
        obj.position = glm::vec3(pos[0], pos[1], pos[2]);
        objects.push_back(obj);
    }

    for (const auto& l : j["pointLights"]) {
        PointLight light;
        light.position = glm::vec3(l["position"][0], l["position"][1], l["position"][2]);
        light.ambient = glm::vec3(l["ambient"][0], l["ambient"][1], l["ambient"][2]);
        light.diffuse = glm::vec3(l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]);
        light.specular = glm::vec3(l["specular"][0], l["specular"][1], l["specular"][2]);
        light.constant = l["constant"];
        light.linear = l["linear"];
        light.quadratic = l["quadratic"];
        pointLights.push_back(light);
    }

    for (const auto& l : j["directionalLights"]) {
        DirectionalLight light;
        light.direction = glm::vec3(l["direction"][0], l["direction"][1], l["direction"][2]);
        light.ambient = glm::vec3(l["ambient"][0], l["ambient"][1], l["ambient"][2]);
        light.diffuse = glm::vec3(l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]);
        light.specular = glm::vec3(l["specular"][0], l["specular"][1], l["specular"][2]);
        dirLights.push_back(light);
    }

    for (const auto& l : j["spotLights"]) {
        SpotLight light;
        light.position = glm::vec3(l["position"][0], l["position"][1], l["position"][2]);
        light.direction = glm::vec3(l["direction"][0], l["direction"][1], l["direction"][2]);
        light.ambient = glm::vec3(l["ambient"][0], l["ambient"][1], l["ambient"][2]);
        light.diffuse = glm::vec3(l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]);
        light.specular = glm::vec3(l["specular"][0], l["specular"][1], l["specular"][2]);
        light.cutOff = l["cutOff"];
        light.outerCutOff = l["outerCutOff"];
        light.constant = l["constant"];
        light.linear = l["linear"];
        light.quadratic = l["quadratic"];
        spotLights.push_back(light);
    }

    for (const auto& cam : j["cameras"]) {
        Camera c;
        c.Position = glm::vec3(cam["position"][0], cam["position"][1], cam["position"][2]);
        c.Front = glm::vec3(cam["front"][0], cam["front"][1], cam["front"][2]);
        c.Up = glm::vec3(cam["up"][0], cam["up"][1], cam["up"][2]);
        c.Yaw = cam["yaw"];
        c.Pitch = cam["pitch"];
        c.Speed = cam["speed"];
        c.Sensitivity = cam["sensitivity"];
        cameras.push_back(c);
    }

    if (j.contains("activeCameraIndex")) {
        activeCameraIndex = j["activeCameraIndex"];
    }

    in.close();
}



