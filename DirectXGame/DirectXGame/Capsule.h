#pragma once
#include "AGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Vector3D.h"

class Capsule : public AGameObject
{
public:
    Capsule(std::string name, void* shaderByteCode, size_t sizeShader);
    ~Capsule();

    void update(float deltaTime) override;
    void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;

private:
    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
    ConstantBuffer* constantBuffer = nullptr;
    float deltaTime = 0.0f;
};
