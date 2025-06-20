#pragma once
#include "AGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DeviceContext.h"

class ScreenQuad : public AGameObject
{
public:
    ScreenQuad(std::string name, const void* shaderByteCode, size_t sizeShader);
    ~ScreenQuad();

    void update(float deltaTime) override;
    void draw(int width, int height, VertexShader* vs, PixelShader* ps) override;

private:
    VertexBuffer* m_vb;
    IndexBuffer* m_ib;
};
