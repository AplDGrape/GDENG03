#pragma once
#include "AGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

class CircleObject : public AGameObject
{
public:
    CircleObject(std::string name, void* shaderByteCode, size_t sizeShader);
    ~CircleObject();

    void update(float deltaTime) override;
    void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;

private:
    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;
    ConstantBuffer* constantBuffer;

    float deltaTime = 0.0f;
};
