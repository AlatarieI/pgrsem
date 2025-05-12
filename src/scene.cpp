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

Camera& Scene::getActiveCamera() {
    return cameras[activeCameraIndex];
}

void Scene::setLightUniforms(GLuint shader) {
    for (auto dirLight : dirLights) {
        dirLight.setUniforms(shader);
    }

    glUniform1i(glGetUniformLocation(shader, "activePointLights"), static_cast<int>(pointLights.size()));
    for (int i = 0; i < pointLights.size(); i++) {
        if (pointLights[i].objectIdx >= 0) {
            pointLights[i].position = objects[pointLights[i].objectIdx].getModelMatrix() * glm::vec4( objects[pointLights[i].objectIdx].position,1.0f);
        }
        pointLights[i].setUniforms(shader, i);
    }

    int activeSpotLights = cameraSpotlightActive ? static_cast<int>(spotLights.size()) : static_cast<int>(spotLights.size())-1;
    glUniform1i(glGetUniformLocation(shader, "activeSpotLights"), activeSpotLights);
    for (int i = 0; i < spotLights.size(); i++) {
        if (i == 0 && cameraSpotlightActive) {
            spotLights[i].position = getActiveCamera().position;
            spotLights[i].direction = getActiveCamera().front;
        } else if (i == 0 && !cameraSpotlightActive) {
            continue;
        } else if (spotLights[i].objectIdx >= 0) {
            glm::mat4 modelMatrix = objects[spotLights[i].objectIdx].getModelMatrix();
            spotLights[i].position = modelMatrix * glm::vec4(objects[spotLights[i].objectIdx].position, 1.0f);
        }
        spotLights[i].setUniforms(shader);
    }
}

void Scene::setObjectUniforms(GLuint shader,  SceneObject& obj) {
    Camera& currentCamera =  getActiveCamera();

    const auto myTime = static_cast<float>(glfwGetTime());
    glUniform1f(glGetUniformLocation(shader, "time"), myTime);

    glm::mat4 model = obj.getModelMatrix();
    glm::mat4 view = currentCamera.getViewMatrix();

    if (obj.name  == "fire") {
        glUniform1i(glGetUniformLocation(shader, "frameCountX"), 12);
        glUniform1i(glGetUniformLocation(shader, "frameCountY"), 6);
        glUniform1f(glGetUniformLocation(shader, "frameRate"), 20.0f);

        glm::vec3 position = glm::vec3(model * glm::vec4(obj.position, 1.0f));
        glm::vec3 direction = glm::normalize(currentCamera.position - position);
        glm::quat rotation = glm::quatLookAt(-direction, glm::vec3(0.0f, 1.0f, 0.0f));
        model *= glm::mat4_cast(rotation);
    }

    if (obj.name == "water") {
        float scrollSpeed = 0.025f;
        float offset = scrollSpeed * myTime;

        float rotationSpeed = 0.01f;
        float angle = myTime * rotationSpeed;

        glm::mat3 texTransform  = glm::mat3(
            glm::vec3( cos(angle), sin(angle), 0.0f),
            glm::vec3(-sin(angle), cos(angle), 0.0f),
            glm::vec3(      offset ,      0.0f , 1.0f)
        );

        glUniformMatrix3fv(glGetUniformLocation(shader, "texTransform"), 1, GL_FALSE, glm::value_ptr(texTransform));
    }

    setLightUniforms(shader);
    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(currentCamera.position));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fogTexture.id);
    glUniform1i(glGetUniformLocation(shader, "useFog"), useFog);

}



void Scene::draw(glm::mat4 projection, float delta) {
    time += delta;

    if (skyBox->shouldBlend) {
        if (time >= 100/skyBox->textures.size()) {
            time = 0.0;
            skyBox->currTextureIdx = (skyBox->currTextureIdx+1) % skyBox->textures.size();
        }
        blend = time / (100/skyBox->textures.size());
    } else {
        blend = 0.5;
    }


    Camera& currentCamera = getActiveCamera();

    if (!useFog)
        skyBox->draw(shaders[skyBox->shaderIdx].id, currentCamera, projection,blend);

    std::vector<SceneObject*> transparent;

    for (auto & obj : objects) {
        if (obj.modelIndex >= 0 && obj.modelIndex < models.size() && obj.shaderIdx >= 0 && obj.shaderIdx < shaders.size()) {
            if (obj.transparent) {
                transparent.push_back(&obj);
                continue;
            }
            GLuint shader = shaders[obj.shaderIdx].id;
            glUseProgram(shader);
            glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


            if (obj.name == "ball") {
                glm::vec3 toCamera = currentCamera.position - obj.position;
                float dist = glm::length(toCamera);

                if (dist < 2.0f) {
                    glm::vec3 awayDir = -glm::normalize(toCamera);
                    obj.position += awayDir * 0.01f;
                    obj.isDirty = true;
                }
            }

            setObjectUniforms(shader,obj);

            models[obj.modelIndex]->draw(shader);
        }
    }

    // Sort back-to-front
    std::sort(transparent.begin(), transparent.end(),
        [&](SceneObject* a, SceneObject* b) {
            glm::vec3 posA = glm::vec3(a->getModelMatrix() * glm::vec4(a->position, 1.0));
            glm::vec3 posB = glm::vec3(b->getModelMatrix() * glm::vec4(b->position, 1.0));
            float distA = glm::length(currentCamera.position - posA);
            float distB = glm::length(currentCamera.position - posB);
            return distA > distB;
        });

    for (auto obj : transparent) {
        if (obj->modelIndex >= 0 && obj->modelIndex < models.size() && obj->shaderIdx >= 0 && obj->shaderIdx < shaders.size()) {
            GLuint shader = shaders[obj->shaderIdx].id;
            glUseProgram(shader);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            setObjectUniforms(shader, *obj);

            models[obj->modelIndex]->draw(shader);

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }

}

void Scene::drawPicking(glm::mat4 projection) {
    Camera& currentCamera = getActiveCamera();
    for (int i = 0; i < objects.size(); i++) {
        SceneObject obj = objects[i];
        if (obj.modelIndex >= 0 && obj.modelIndex < models.size()) {
            GLuint shader = shaders[pickingShaderIdx].id;
            glUseProgram(shader);
            glm::mat4 model = obj.getModelMatrix();
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glm::mat4 view = currentCamera.getViewMatrix();
            glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1f(glGetUniformLocation(shader, "id"), static_cast<float>(i + 1));

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

    j["pickingShaderIdx"] = pickingShaderIdx;

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

    if (skyBox) {
        j["skyBox"] = {
            {"shaderIdx", skyBox->shaderIdx}
        };
        for (const auto& texture : skyBox->textures) {
            j["skyBox"]["texturePaths"].push_back(texture.path);
        }

    }

    for (const auto& obj : objects) {
        j["objects"].push_back({
            {"name", obj.name},
            {"modelIndex", obj.modelIndex},
            {"position", {obj.position.x, obj.position.y, obj.position.z}},
            {"rotation", {obj.rotation.x, obj.rotation.y, obj.rotation.z}},
            {"scale", {obj.scale.x, obj.scale.y, obj.scale.z}},
            {"parentIdx", obj.parentIdx},
            {"shaderIdx", obj.shaderIdx},
            {"transparent", obj.transparent}
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
            {"quadratic", light.quadratic},
            {"objectIdx", light.objectIdx}
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
            {"quadratic", light.quadratic},
            {"objectIdx", light.objectIdx}
        });
    }

    for (const auto& cam : cameras) {
        j["cameras"].push_back({
            {"position", {cam.position.x, cam.position.y, cam.position.z}},
            {"front", {cam.front.x, cam.front.y, cam.front.z}},
            {"up", {cam.up.x, cam.up.y, cam.up.z}},
            {"yaw", cam.rotationY},
            {"pitch", cam.rotationX},
            {"speed", cam.speed},
            {"sensitivity", cam.sensitivity},
            {"shouldMove", cam.shouldMove}
        });
    }

    j["activeCameraIndex"] = activeCameraIndex;
    j["fogTexture"] = fogTexture.path;

    std::ofstream out(file);
    if (!out) {
        std::cerr << "Failed to open file for saving: " << file << std::endl;
        return;
    }
    out << j.dump(4);
    out.close();
    std::cout << "Scene saved to " << file << std::endl;
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

    if (j.contains("pickingShaderIdx")) {
        pickingShaderIdx = j["pickingShaderIdx"];
    }

    std::cout << "Shaders loaded" << std::endl;

    for (const auto& m : j["models"]) {
        std::string path = m["path"];
        bool flipUV = m["flipUV"];
        bool gamma = m["gamma"];
        addModel(path, flipUV, gamma);
    }

    std::cout << "Models loaded" << std::endl;

    if (j.contains("skyBox")) {
        auto dome = j["skyBox"];
        std::vector<std::string> texturePaths;
        for (const auto& o : dome["texturePaths"]) {
            texturePaths.push_back(o);
        }
        skyBox = new SkyBox(texturePaths, dome["shaderIdx"]);
    }

    std::cout << "Sky box loaded" << std::endl;

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

        obj.shaderIdx = o["shaderIdx"];
        obj.parentIdx = o["parentIdx"];
        if (o.contains("transparent")) {
            obj.transparent = o["transparent"];
        }

        objects.push_back(obj);
    }

    for (auto& object : objects) {
        if (object.parentIdx >= 0 && object.parentIdx < objects.size()) {
            object.parent = &objects[object.parentIdx];
            object.parent->children.push_back(&object);
        } else {
            object.parent = nullptr;
        }
    }

    std::cout << "Objects loaded" << std::endl;

    for (const auto& l : j["pointLights"]) {
        PointLight light{};
        light.position = glm::vec3(l["position"][0], l["position"][1], l["position"][2]);
        light.ambient = glm::vec3(l["ambient"][0], l["ambient"][1], l["ambient"][2]);
        light.diffuse = glm::vec3(l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]);
        light.specular = glm::vec3(l["specular"][0], l["specular"][1], l["specular"][2]);
        light.constant = l["constant"];
        light.linear = l["linear"];
        light.quadratic = l["quadratic"];
        light.objectIdx = l["objectIdx"];
        pointLights.push_back(light);
    }

    std::cout << "Point lights loaded" << std::endl;

    for (const auto& l : j["directionalLights"]) {
        DirectionalLight light{};
        light.direction = glm::vec3(l["direction"][0], l["direction"][1], l["direction"][2]);
        light.ambient = glm::vec3(l["ambient"][0], l["ambient"][1], l["ambient"][2]);
        light.diffuse = glm::vec3(l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]);
        light.specular = glm::vec3(l["specular"][0], l["specular"][1], l["specular"][2]);
        dirLights.push_back(light);
    }

    std::cout << "Dir lights loaded" << std::endl;




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
        light.objectIdx = l["objectIdx"];
        spotLights.push_back(light);
    }

    if (spotLights.size() < 1) {
        SpotLight cameraSpotLight{};
        cameraSpotLight.objectIdx = -1;
        cameraSpotLight.ambient = glm::vec3(0.1f);
        cameraSpotLight.diffuse = glm::vec3(0.8f);
        cameraSpotLight.specular = glm::vec3(1.0f);
        cameraSpotLight.cutOff = 12.5f;
        cameraSpotLight.outerCutOff = 15.0f;
        cameraSpotLight.constant = 1.0f;
        cameraSpotLight.linear = 0.09f;
        cameraSpotLight.quadratic = 0.032f;
        cameraSpotLight.direction = glm::vec3(1.0f, 0.0f, 0.0f);
        cameraSpotLight.position = glm::vec3(0.0f);
        spotLights.push_back(cameraSpotLight);
    }

    std::cout << "Spotlights loaded" << std::endl;

    for (const auto& cam : j["cameras"]) {
        Camera c{};
        c.position = glm::vec3(cam["position"][0], cam["position"][1], cam["position"][2]);
        c.front = glm::vec3(cam["front"][0], cam["front"][1], cam["front"][2]);
        c.up = glm::vec3(cam["up"][0], cam["up"][1], cam["up"][2]);
        c.rotationY = cam["yaw"];
        c.rotationX = cam["pitch"];
        c.speed = cam["speed"];
        c.sensitivity = cam["sensitivity"];
        c.shouldMove = cam["shouldMove"];
        cameras.push_back(c);
    }

    std::cout << "Cameras loaded" << std::endl;

    if (j.contains("activeCameraIndex")) {
        activeCameraIndex = j["activeCameraIndex"];
    }

    if (j.contains("fogTexture")) {
        fogTexture.path = j["fogTexture"];
        fogTexture.type = "diffuse";
        fogTexture.id = load_texture(fogTexture.path.c_str());
    }

    in.close();
}



