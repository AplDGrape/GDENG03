#pragma once
#include <vector>

struct vec3
{
    float x, y, z;
};

struct vertex
{
    vec3 position;
    vec3 position1;
    vec3 color;
    vec3 color1;
};

class RenderMultipleQuad
{
public:
    static void initialize();
    static void destroy();
    static RenderMultipleQuad* getInstance();

    // Return pointer to vertex list
    const vertex* getVertexList() const;

    // Return number of vertices in the list
    size_t getVertexCount() const;

private:
    RenderMultipleQuad();
    ~RenderMultipleQuad();

    static RenderMultipleQuad* sharedInstance;

    // Internal storage for vertices
    std::vector<vertex> m_vertexList;

    void buildVertexList();
};
