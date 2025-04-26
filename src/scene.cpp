#include "scene.h"

using json = nlohmann::json;

int Scene::addShader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource) {
    Shader shader = Shader(vertexShaderSource, fragmentShaderSource);
    this->shaders.push_back(shader);
    return shaders.size() - 1;
}

int Scene::addShader(Shader shader) {
    this->shaders.push_back(shader);
    return shaders.size() - 1;
}

int Scene::addModel(std::string path, bool flipUV, bool gamma) {
    for (int i = 0; i < modelPaths.size(); ++i) {
        if (modelPaths[i] == path)
            return i;
    }

    Model* newModel = new Model(path, flipUV, gamma);
    models.push_back(newModel);
    modelPaths.push_back(path);
    return models.size() - 1;
}

int Scene::addModel(Model* model) {
    models.push_back(model);
    modelPaths.push_back("");
    return models.size() - 1;
}

void Scene::addObject(std::string name, int shaderIndex, int modelIndex, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
    if (modelIndex < 0 || modelIndex >= models.size()) return;
    SceneObject obj = SceneObject(shaderIndex,  modelIndex, position, rotation, scale);
    obj.name = name;
    objects.push_back(obj);
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
    return &cameras[activeCameraIndex];
}

void Scene::setLightUniforms(GLuint shader) {
    for (auto dirLight : dirLights) {
        dirLight.setUniforms(shader);
    }

    for (int i = 0; i < pointLights.size(); i++) {
        pointLights[i].setUniforms(shader, i);
    }

    for (auto spot : spotLights) {
        spot.setUniforms(shader);
    }
}

void Scene::draw(glm::mat4 projection) {
    skyDome->draw(shaders[skyDome->shaderIdx].id, getActiveCamera(), projection);
    for (const auto& obj : objects) {
        if (obj.modelIndex >= 0 && obj.modelIndex < models.size() && obj.shaderIdx >= 0 && obj.shaderIdx < shaders.size()) {
            GLuint shader = shaders[obj.shaderIdx].id;
            glUseProgram(shader);
            setLightUniforms(shader);
            auto model = obj.getModelMatrix();
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            auto view = getActiveCamera()->getViewMatrix();
            glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            models[obj.modelIndex]->draw(shader);
        }
    }
}

void Scene::save(const std::string& file) {
    json j;

    for (const auto& shader : shaders) {
        j["shaders"].push_back({
            {"vertexShaderSource", shader.vertexShaderSource},
            {"fragmentShaderSource", shader.fragmentShaderSource},
        });
    }

    for (int i = 0; i < models.size(); ++i) {
        if (!modelPaths[i].empty()) {
            Model* model = models[i];
            j["models"].push_back({
                {"path", modelPaths[i]},
                {"flipUV", model->flipUV},
                {"gamma", model->gammaCorrection}
            });
        }

    }

    if (skyDome) {
        j["skyDome"] = {
            {"texturePath", skyDome->texture.path},
            {"shaderIdx", skyDome->shaderIdx}
        };
    }

    for (const auto& obj : objects) {
        j["objects"].push_back({
            {"name", obj.name},
            {"modelIndex", obj.modelIndex},
            {"position", {obj.position.x, obj.position.y, obj.position.z}},
            {"rotation", {obj.rotation.x, obj.rotation.y, obj.rotation.z}},
            {"scale", {obj.scale.x, obj.scale.y, obj.scale.z}},
            {"parentIdx", obj.parentIdx},
            {"shaderIdx", obj.shaderIdx}
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

    shaders.clear();
    modelPaths.clear();
    models.clear();
    objects.clear();
    pointLights.clear();
    dirLights.clear();
    spotLights.clear();
    cameras.clear();

    for (const auto& m : j["shaders"]) {
        std::string vertexShaderSource = m["vertexShaderSource"];
        std::string fragmentShaderSource = m["fragmentShaderSource"];
        addShader(vertexShaderSource, fragmentShaderSource);
    }

    for (const auto& m : j["models"]) {
        std::string path = m["path"];
        bool flipUV = m["flipUV"];
        bool gamma = m["gamma"];
        addModel(path, flipUV, gamma);
    }

    if (j.contains("skyDome")) {
        auto dome = j["skyDome"];
        skyDome = new SkyDome(dome["texturePath"], dome["shaderIdx"]);
    }

    for (const auto& o : j["objects"]) {
        SceneObject obj;
        obj.name = o["name"];
        obj.modelIndex = o["modelIndex"];
        auto pos = o["position"];
        auto rot = o["rotation"];
        auto scale = o["scale"];
        obj.position = glm::vec3(pos[0], pos[1], pos[2]);
        obj.rotation = glm::vec3(rot[0], rot[1], rot[2]);
        obj.scale = glm::vec3(scale[0], scale[1], scale[2]);

        obj.parentIdx = o["parentIdx"];
        if (obj.parentIdx >= 0 && obj.parentIdx < objects.size())
            obj.parent = &objects[obj.parentIdx];
        else
            obj.parent = nullptr;

        obj.shaderIdx = o["shaderIdx"];

        objects.push_back(obj);
    }

    for (const auto& l : j["pointLights"]) {
        PointLight light{};
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
        DirectionalLight light{};
        light.direction = glm::vec3(l["direction"][0], l["direction"][1], l["direction"][2]);
        light.ambient = glm::vec3(l["ambient"][0], l["ambient"][1], l["ambient"][2]);
        light.diffuse = glm::vec3(l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]);
        light.specular = glm::vec3(l["specular"][0], l["specular"][1], l["specular"][2]);
        dirLights.push_back(light);
    }

    for (const auto& l : j["spotLights"]) {
        SpotLight light{};
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
        Camera c{};
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



