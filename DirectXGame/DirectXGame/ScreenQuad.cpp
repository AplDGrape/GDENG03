#include "ScreenQuad.h"
#include "GraphicsEngine.h"

struct FSVertex
{
    Vector3D position;
    Vector3D uv;
};

ScreenQuad::ScreenQuad(std::string name, const void* shaderByteCode, size_t sizeShader)
    : AGameObject(name)
{
    FSVertex vertices[] = {
        {{-1.0f, -1.0f, 0.0f}, {0, 1, 0}},
        {{-1.0f,  1.0f, 0.0f}, {0, 0, 0}},
        {{ 1.0f,  1.0f, 0.0f}, {1, 0, 0}},
        {{ 1.0f, -1.0f, 0.0f}, {1, 1, 0}}
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    m_vb = GraphicsEngine::get()->createVertexBuffer();
    m_vb->load((void*)vertices, sizeof(FSVertex), ARRAYSIZE(vertices), (void*)shaderByteCode, sizeShader);

    m_ib = GraphicsEngine::get()->createIndexBuffer();
    m_ib->load(indices, ARRAYSIZE(indices));
}

void ScreenQuad::update(float deltaTime) {}

void ScreenQuad::draw(int width, int height, VertexShader* vs, PixelShader* ps)
{
    auto context = GraphicsEngine::get()->getImmediateDeviceContext();

    context->setVertexShader(vs);
    context->setPixelShader(ps);

    context->setVertexBuffer(m_vb);
    context->setIndexBuffer(m_ib);

    context->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);
}

ScreenQuad::~ScreenQuad()
{
    m_vb->release();
    m_ib->release();
}
