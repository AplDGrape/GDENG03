#pragma once
#include <vector>
#include "Vector3D.h"

//struct vec3
//{
//    float x, y, z;
//};

struct vertex
{
    /*vec3 position;
    vec3 position1;
    vec3 color;
    vec3 color1;*/
    Vector3D position;
    Vector3D position1;
    Vector3D color;
    Vector3D color1;
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

    void setRenderShape(bool isQuad); // Add this method to switch between shapes
private:
    RenderMultipleQuad();
    ~RenderMultipleQuad();

    static RenderMultipleQuad* sharedInstance;

    // Internal storage for vertices
    std::vector<vertex> m_vertexList;

    void buildVertexList();

    void buildTriangle(); // Method to build triangle vertices
    void buildQuad();     // Method to build quad vertices
};
