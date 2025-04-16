//
// Created by luky9 on 14.04.2025.
//

#include "scene.h"

Scene::Scene() {
    rootNodes = std::vector<SceneNode*>();
};

void Scene::addNode(GameObject* gameObject) {
    rootNodes.push_back(new SceneNode(gameObject, nullptr));
}

void Scene::addNode(SceneNode* node) {
    rootNodes.push_back(node);
}

void Scene::update(float deltaTime) {
    std::queue<SceneNode*> queue;

    for (SceneNode* node : rootNodes) {
        queue.push(node);
    }

    while (!queue.empty()) {
        SceneNode* sceneNode = queue.front();
        queue.pop();
        sceneNode->gameObject->update(deltaTime);
        queue.push(sceneNode->children.front());
    }
}

void Scene::draw() {
    for (SceneNode* node : rootNodes) {
        node->gameObject->draw();
        drawChildren(node, node->gameObject->getModelMatrix());
    }
}

void Scene::drawChildren(SceneNode* node, glm::mat4 modelMatrix) {
    for (SceneNode* childNode : node->children) {
        childNode->gameObject->draw(modelMatrix);
        if (!childNode->children.empty())
            drawChildren(childNode, modelMatrix*node->gameObject->getModelMatrix());
    }
}


SceneNode::SceneNode(GameObject *gameObject, SceneNode *parent) {
    this->gameObject = gameObject;
    this->parent = parent;
}




