#include "RenderMultipleQuad.h"
#include <iostream>

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
        Vector3D bottomLeft = { -quadSize + offsetX, -quadSize, 0.0f };
        Vector3D topLeft = { -quadSize + offsetX,  quadSize, 0.0f };
        Vector3D bottomRight = { quadSize + offsetX, -quadSize, 0.0f };
        Vector3D topRight = { quadSize + offsetX,  quadSize, 0.0f };

        Vector3D color = { 0.3f * i, 1.0f - 0.3f * i, 0.5f }; // unique color per quad

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

void RenderMultipleQuad::buildTriangle()
{
    // Define vertices for a single triangle
    m_vertexList = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.0f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    std::cout << "Triangle\n";
}

void RenderMultipleQuad::buildQuad()
{
    const int quadCount = 3;
    const float quadSize = 0.3f;
    const float spacing = 0.4f;

    for (int i = 0; i < quadCount; ++i)
    {
        float offsetX = (i - 1) * spacing;

        Vector3D bottomLeft = { -quadSize + offsetX, -quadSize, 0.0f };
        Vector3D topLeft = { -quadSize + offsetX,  quadSize, 0.0f };
        Vector3D bottomRight = { quadSize + offsetX, -quadSize, 0.0f };
        Vector3D topRight = { quadSize + offsetX,  quadSize, 0.0f };

        Vector3D color = { 0.3f * i, 1.0f - 0.3f * i, 0.5f };

        m_vertexList.push_back({ bottomLeft, color });
        m_vertexList.push_back({ topLeft, color });
        m_vertexList.push_back({ bottomRight, color });

        m_vertexList.push_back({ bottomRight, color });
        m_vertexList.push_back({ topLeft, color });
        m_vertexList.push_back({ topRight, color });
    }

    std::cout << "Quad\n";
}


const vertex* RenderMultipleQuad::getVertexList() const
{
    return m_vertexList.data();
}

size_t RenderMultipleQuad::getVertexCount() const
{
    return m_vertexList.size();
}

void RenderMultipleQuad::setRenderShape(bool isQuad)
{
    m_vertexList.clear();
    if (isQuad)
        buildQuad();
    else
        buildTriangle();
}
