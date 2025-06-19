#pragma once
#include <vector>
#include <string>
#include "AGameObject.h"
#include <Windows.h>

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();
    static void destroy();

    void addObject(AGameObject* object);
    AGameObject* findObjectByName(const std::string& name);
    void updateAll(float deltaTime);
    void drawAll(int width, int height, VertexShader* vs, PixelShader* ps);

private:
    GameObjectManager() = default;
    ~GameObjectManager();
    static GameObjectManager* instance;

    std::vector<AGameObject*> objectList;
};