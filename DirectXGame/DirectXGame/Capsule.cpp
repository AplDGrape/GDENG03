#include "Capsule.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "AppWindow.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Capsule::Capsule(std::string name, void* shaderByteCode, size_t sizeShader) : AGameObject(name)
{
    const int stacks = 10;
    const int slices = 20;
    const float radius = 0.3f;
    const float halfHeight = 0.5f;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices for the top hemisphere
    for (int i = 0; i <= stacks; ++i) {
        float v = i / (float)stacks;
        float phi = v * (M_PI / 2.0f); // Top half

        for (int j = 0; j <= slices; ++j) {
            float u = j / (float)slices;
            float theta = u * (2.0f * M_PI);

            float x = cosf(theta) * sinf(phi);
            float y = cosf(phi);
            float z = sinf(theta) * sinf(phi);

            Vector3D pos = Vector3D(x, y + halfHeight, z) * radius;
            vertices.push_back({ pos, Vector3D(1, 0, 0), Vector3D(0, 0, 1) });
        }
    }

    // Cylinder body
    for (int i = 0; i <= 1; ++i) {
        float h = -halfHeight + i;
        for (int j = 0; j <= slices; ++j) {
            float theta = (j / (float)slices) * 2.0f * M_PI;
            float x = cosf(theta);
            float z = sinf(theta);
            Vector3D pos = Vector3D(x, h, z) * radius;
            vertices.push_back({ pos, Vector3D(0, 1, 0), Vector3D(1, 1, 0) });
        }
    }

    // Bottom hemisphere
    for (int i = 0; i <= stacks; ++i) {
        float v = i / (float)stacks;
        float phi = v * (M_PI / 2.0f); // Bottom half

        for (int j = 0; j <= slices; ++j) {
            float u = j / (float)slices;
            float theta = u * (2.0f * M_PI);

            float x = cosf(theta) * sinf(phi);
            float y = -cosf(phi);
            float z = sinf(theta) * sinf(phi);

            Vector3D pos = Vector3D(x, y - halfHeight, z) * radius;
            vertices.push_back({ pos, Vector3D(0, 0, 1), Vector3D(1, 0, 1) });
        }
    }

    // (You can fill in indices for hemisphere and cylinder triangulation, or use drawIndexedTriangleList with simplified quads)

    this->vertexBuffer = GraphicsEngine::get()->createVertexBuffer();
    this->vertexBuffer->load(vertices.data(), sizeof(Vertex), (UINT)vertices.size(), shaderByteCode, sizeShader);

    // Just a dummy quad index (for now)
    unsigned int simpleIndices[] = { 0, 1, 2, 2, 3, 0 };
    this->indexBuffer = GraphicsEngine::get()->createIndexBuffer();
    this->indexBuffer->load(simpleIndices, ARRAYSIZE(simpleIndices));

    CBData cbData = {};
    cbData.time = 0;
    this->constantBuffer = GraphicsEngine::get()->createConstantBuffer();
    this->constantBuffer->load(&cbData, sizeof(CBData));
}

Capsule::~Capsule()
{
    this->vertexBuffer->release();
    this->indexBuffer->release();
}

void Capsule::update(float deltaTime)
{
    this->deltaTime = deltaTime;
}

void Capsule::draw(int width, int height, VertexShader* vs, PixelShader* ps)
{
    DeviceContext* ctx = GraphicsEngine::get()->getImmediateDeviceContext();

    CBData cbData = {};
    cbData.worldMatrix.setTranslation(this->getLocalPosition());
    cbData.viewMatrix.setIdentity();
    cbData.projMatrix.setPerspectiveFovLH(1.57f, (float)width / height, 0.1f, 100.0f);

    this->constantBuffer->update(ctx, &cbData);

    ctx->setInputLayout(vs->getInputLayout());
    ctx->setConstantBuffer(vs, constantBuffer);
    ctx->setConstantBuffer(ps, constantBuffer);

    ctx->setVertexShader(vs);
    ctx->setPixelShader(ps);

    ctx->setVertexBuffer(vertexBuffer);
    ctx->setIndexBuffer(indexBuffer);
    ctx->drawIndexedTriangleList(indexBuffer->getSizeIndexList(), 0, 0);
}
