#include "CircleObject.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include <cmath>

CircleObject::CircleObject(std::string name, void* shaderByteCode, size_t sizeShader)
    : AGameObject(name)
{
    const int numSegments = 64;
    const float radius = 0.5f;

    // Vertices
    Vertex* vertices = new Vertex[numSegments + 1];
    vertices[0] = { Vector3D(0, 0, 0), Vector3D(1, 0, 0), Vector3D(0, 0, 0) }; // center vertex

    for (int i = 0; i < numSegments; ++i)
    {
        float angle = 2.0f * 3.14159265f * i / numSegments;
        float x = cosf(angle) * radius;
        float y = sinf(angle) * radius;
        vertices[i + 1] = { Vector3D(x, y, 0), Vector3D(0, 1, 0), Vector3D(0, 0, 0) };
    }

    // Indices
    unsigned int* indices = new unsigned int[numSegments * 3];
    for (int i = 0; i < numSegments; ++i)
    {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i + 2 > numSegments) ? 1 : i + 2;
    }

    vertexBuffer = GraphicsEngine::get()->createVertexBuffer();
    vertexBuffer->load(vertices, sizeof(Vertex), numSegments + 1, shaderByteCode, sizeShader);

    indexBuffer = GraphicsEngine::get()->createIndexBuffer();
    indexBuffer->load(indices, numSegments * 3);

    CBData cbData = {};
    constantBuffer = GraphicsEngine::get()->createConstantBuffer();
    constantBuffer->load(&cbData, sizeof(CBData));

    delete[] vertices;
    delete[] indices;
}

CircleObject::~CircleObject()
{
    vertexBuffer->release();
    indexBuffer->release();
    constantBuffer->release();
}

void CircleObject::update(float deltaTime)
{
    this->deltaTime = deltaTime;
}

void CircleObject::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
    DeviceContext* context = GraphicsEngine::get()->getImmediateDeviceContext();

    Matrix4x4 worldMatrix;
    worldMatrix.setIdentity();

    Matrix4x4 translation;
    translation.setTranslation(this->getLocalPosition());

    Matrix4x4 scale;
    scale.setScale(this->getLocalScale());

    Matrix4x4 rotationZ, rotationX, rotationY;
    rotationZ.setRotationZ(this->getLocalRotation().m_z);
    rotationX.setRotationX(this->getLocalRotation().m_x);
    rotationY.setRotationY(this->getLocalRotation().m_y);

    Matrix4x4 rotMatrix;
    rotMatrix.setIdentity();
    rotMatrix = rotMatrix.multiplyTo(rotationX.multiplyTo(rotationY.multiplyTo(rotationZ)));

    worldMatrix = worldMatrix.multiplyTo(scale.multiplyTo(rotMatrix));
    worldMatrix = worldMatrix.multiplyTo(translation);

    CBData cbData = {};
    cbData.worldMatrix = worldMatrix;
    cbData.viewMatrix.setIdentity();
    cbData.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -4.0f, 4.0f);

    constantBuffer->update(context, &cbData);
    context->setConstantBuffer(vertexShader, constantBuffer);
    context->setConstantBuffer(pixelShader, constantBuffer);

    context->setVertexBuffer(vertexBuffer);
    context->setIndexBuffer(indexBuffer);

    context->drawIndexedTriangleList(indexBuffer->getSizeIndexList(), 0, 0);
}
