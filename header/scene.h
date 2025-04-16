//
// Created by luky9 on 14.04.2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <queue>

#include "game_object.h"

class SceneNode {
public:
    std::vector<SceneNode*> children;
    SceneNode* parent;

    GameObject* gameObject;

    SceneNode(GameObject* gameObject, SceneNode* parent = nullptr);

};

class Scene {
public:
    vector<SceneNode*> rootNodes;

    Scene();

    void addNode(GameObject* gameObject);

    void addNode(SceneNode* node);

    void update(float deltaTime);

    void draw();

    void drawChildren(SceneNode* node, glm::mat4 modelMatrix);
};



#endif //SCENE_H
