#pragma once
#include "AGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

class SimplePlane : public AGameObject
{
public:
    SimplePlane(string name, void* shaderByteCode, size_t sizeShader);
    ~SimplePlane();

    void update(float deltaTime) override;
    void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;

private:
    VertexBuffer* m_vb;
    IndexBuffer* m_ib;
    ConstantBuffer* m_cb;
};
