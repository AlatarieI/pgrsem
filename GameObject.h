//
// Created by skorelu1 on 31.03.2025.
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <string>


class GameObject {
private:
    unsigned int mVBO, mVAO, mEBO, mTexture;
public:
    GameObject(std::string modelFilePath, bool element);

    GameObject(float vertices[], unsigned int indices[]);

    void Draw();

};



#endif //GAMEOBJECT_H
