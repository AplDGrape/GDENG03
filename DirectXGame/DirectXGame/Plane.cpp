#include "Plane.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"

Plane::Plane(string name, void* shaderByteCode, size_t sizeShader) : AGameObject(name)
{
    // A simple quad on the X-Y plane (Z = 0)
    Vertex quad[] = {
        { Vector3D(-0.5f, 0.0f, -0.5f), Vector3D(1, 0, 0), Vector3D(0, 0, 0) }, // bottom-left
        { Vector3D(-0.5f,  0.0f, 0.5f), Vector3D(0, 1, 0), Vector3D(0, 1, 0) }, // top-left
        { Vector3D(0.5f,  0.0f, 0.5f), Vector3D(0, 0, 1), Vector3D(1, 1, 0) }, // top-right
        { Vector3D(0.5f, 0.0f, -0.5f), Vector3D(1, 1, 0), Vector3D(1, 0, 0) }  // bottom-right
    };

    this->vertexBuffer = GraphicsEngine::get()->createVertexBuffer();
    this->vertexBuffer->load(quad, sizeof(Vertex), ARRAYSIZE(quad), shaderByteCode, sizeShader);

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    this->indexBuffer = GraphicsEngine::get()->createIndexBuffer();
    this->indexBuffer->load(indices, ARRAYSIZE(indices));

    CBData cbData = {};
    this->constantBuffer = GraphicsEngine::get()->createConstantBuffer();
    this->constantBuffer->load(&cbData, sizeof(CBData));
}

Plane::~Plane()
{
    this->vertexBuffer->release();
    this->indexBuffer->release();
    this->constantBuffer->release();
}

void Plane::update(float deltaTime)
{
    this->deltaTime = deltaTime;
}

void Plane::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
    GraphicsEngine* gfx = GraphicsEngine::get();
    DeviceContext* ctx = gfx->getImmediateDeviceContext();

    CBData cbData = {};
    Matrix4x4 worldMatrix, scaleMatrix, rotMatrix, transMatrix;
    worldMatrix.setIdentity();
    scaleMatrix.setScale(getLocalScale());
    transMatrix.setTranslation(getLocalPosition());

    Vector3D rot = getLocalRotation();
    Matrix4x4 xRot, yRot, zRot;
    xRot.setRotationX(rot.m_x);
    yRot.setRotationY(rot.m_y);
    zRot.setRotationZ(rot.m_z);
    rotMatrix = xRot.multiplyTo(yRot.multiplyTo(zRot));

    worldMatrix = worldMatrix.multiplyTo(scaleMatrix.multiplyTo(rotMatrix));
    worldMatrix = worldMatrix.multiplyTo(transMatrix);

    cbData.worldMatrix = worldMatrix;
    cbData.viewMatrix.setIdentity();
    cbData.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -4.0f, 4.0f);

    constantBuffer->update(ctx, &cbData);
    ctx->setConstantBuffer(vertexShader, constantBuffer);
    ctx->setConstantBuffer(pixelShader, constantBuffer);

    ctx->setIndexBuffer(indexBuffer);
    ctx->setVertexBuffer(vertexBuffer);
    ctx->drawIndexedTriangleList(indexBuffer->getSizeIndexList(), 0, 0);
}
