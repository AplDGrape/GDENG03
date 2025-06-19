#include "GameObjectManager.h"

GameObjectManager* GameObjectManager::instance = nullptr;

GameObjectManager* GameObjectManager::getInstance()
{
    if (!instance)
        instance = new GameObjectManager();
    return instance;
}

void GameObjectManager::destroy()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

GameObjectManager::~GameObjectManager()
{
    for (AGameObject* obj : objectList)
        delete obj;
    objectList.clear();
}

void GameObjectManager::addObject(AGameObject* object)
{
    objectList.push_back(object);
}

AGameObject* GameObjectManager::findObjectByName(const std::string& name)
{
    for (AGameObject* obj : objectList)
        if (obj->getName() == name)
            return obj;
    return nullptr;
}

void GameObjectManager::updateAll(float deltaTime)
{
    for (AGameObject* obj : objectList)
        obj->update(deltaTime);
}

void GameObjectManager::drawAll(int width, int height, VertexShader* vs, PixelShader* ps)
{
    for (AGameObject* obj : objectList) {
        obj->draw(width, height, vs, ps);
        OutputDebugStringA(("[OK] Drew cube: " + obj->getName() + "\n").c_str());
    }
}