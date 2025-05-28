#include "RenderMultipleQuad.h"

RenderMultipleQuad* RenderMultipleQuad::sharedInstance = nullptr;

RenderMultipleQuad::RenderMultipleQuad()
{
    buildVertexList();
}

RenderMultipleQuad::~RenderMultipleQuad()
{
}

void RenderMultipleQuad::initialize()
{
    if (!sharedInstance)
        sharedInstance = new RenderMultipleQuad();
}

void RenderMultipleQuad::destroy()
{
    if (sharedInstance)
    {
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

RenderMultipleQuad* RenderMultipleQuad::getInstance()
{
    return sharedInstance;
}

//void RenderMultipleQuad::buildVertexList()
//{
//    // Fill the vector with your vertex data here
//    // This is the data you had in AppWindow
//
//    m_vertexList = {
//        // RECT GREEN (4 vertices for one quad)
//        { { -0.5f, -0.5f, 0.0f }, { 0, 1, 0 } },
//        { { -0.5f,  0.5f, 0.0f }, { 0, 1, 0 } },
//        { {  0.5f, -0.5f, 0.0f }, { 0, 1, 0 } },
//        { {  0.5f,  0.5f, 0.0f }, { 0, 0, 0 } }
//    };
//}

void RenderMultipleQuad::buildVertexList()
{
    const int quadCount = 3;
    const float quadSize = 0.3f;
    const float spacing = 0.4f; // enough spacing to avoid overlap

    for (int i = 0; i < quadCount; ++i)
    {
        float offsetX = (i - 1) * spacing; // spread them left, center, right

        // Quad corner positions (2 triangles per quad)
        vec3 bottomLeft = { -quadSize + offsetX, -quadSize, 0.0f };
        vec3 topLeft = { -quadSize + offsetX,  quadSize, 0.0f };
        vec3 bottomRight = { quadSize + offsetX, -quadSize, 0.0f };
        vec3 topRight = { quadSize + offsetX,  quadSize, 0.0f };

        vec3 color = { 0.3f * i, 1.0f - 0.3f * i, 0.5f }; // unique color per quad

        // Triangle 1
        m_vertexList.push_back({ bottomLeft,  color });
        m_vertexList.push_back({ topLeft,     color });
        m_vertexList.push_back({ bottomRight, color });

        // Triangle 2
        m_vertexList.push_back({ bottomRight, color });
        m_vertexList.push_back({ topLeft,     color });
        m_vertexList.push_back({ topRight,    color });
    }
}


const vertex* RenderMultipleQuad::getVertexList() const
{
    return m_vertexList.data();
}

size_t RenderMultipleQuad::getVertexCount() const
{
    return m_vertexList.size();
}
