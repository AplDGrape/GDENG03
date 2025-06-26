#include "SimplePlane.h"

SimplePlane::SimplePlane(string name, void* shaderByteCode, size_t sizeShader)
    : AGameObject(name)
{
    // Flat XZ quad
    Vertex vertex_list[] =
    {
        { Vector3D(-10.5f, -0.5f, -10.5f), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.2f, 0.2f, 0.2f) },
        { Vector3D(-10.5f, -0.5f,  10.5f), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.2f, 0.2f, 0.2f) },
        { Vector3D(10.5f, -0.5f,  10.5f), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.2f, 0.2f, 0.2f) },
        { Vector3D(10.5f, -0.5f, -10.5f), Vector3D(0.2f, 0.2f, 0.2f), Vector3D(0.2f, 0.2f, 0.2f) }
    };

    unsigned int index_list[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    m_vb = GraphicsEngine::get()->createVertexBuffer();
    m_vb->load(vertex_list, sizeof(Vertex), ARRAYSIZE(vertex_list), shaderByteCode, sizeShader);

    m_ib = GraphicsEngine::get()->createIndexBuffer();
    m_ib->load(index_list, ARRAYSIZE(index_list));

    m_cb = GraphicsEngine::get()->createConstantBuffer();
    AGameObject::CBData data = {};
    m_cb->load(&data, sizeof(CBData));
}

SimplePlane::~SimplePlane()
{
    if (m_vb) m_vb->release();
    if (m_ib) m_ib->release();
}

void SimplePlane::update(float deltaTime)
{
    // Add logic if needed
}

void SimplePlane::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
    GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
    GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(m_ib);
    GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(vertexShader);
    GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(pixelShader);

    // Drawing without transformation (no constant buffer)
    GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangleList(
        m_ib->getSizeIndexList(), 0, 0);
    
    //// Compose world matrix
    //Matrix4x4 worldMatrix;
    //worldMatrix.setIdentity();
    //worldMatrix.setScale(getLocalScale());

    //Matrix4x4 temp;
    //temp.setIdentity();
    //temp.setRotationX(getLocalRotation().m_x);
    //worldMatrix *= temp;

    //temp.setIdentity();
    //temp.setRotationY(getLocalRotation().m_y);
    //worldMatrix *= temp;

    //temp.setIdentity();
    //temp.setRotationZ(getLocalRotation().m_z);
    //worldMatrix *= temp;

    //temp.setIdentity();
    //temp.setTranslation(getLocalPosition());
    //worldMatrix *= temp;

    //// Fill constant buffer
    //AGameObject::CBData cbdata = {};
    //cbdata.worldMatrix = worldMatrix;
    ////cbdata.viewMatrix = AppWindow::getViewMatrix();   // You need to pass this in
    //cbdata.viewMatrix.setIdentity();
    ////cbdata.projMatrix = AppWindow::getProjMatrix();   // You need to pass this in
    //cbdata.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -10.0f, 10.0f);
    //cbdata.time = 0.0f;

    //m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cbdata);

    //// Set pipeline
    //auto ctx = GraphicsEngine::get()->getImmediateDeviceContext();
    //ctx->setConstantBuffer(vertexShader, m_cb);
    //ctx->setConstantBuffer(pixelShader, m_cb);

    //ctx->setVertexShader(vertexShader);
    //ctx->setPixelShader(pixelShader);
    //ctx->setVertexBuffer(m_vb);
    //ctx->setIndexBuffer(m_ib);

    //ctx->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);
}
