#pragma once
#include <d3d11.h>

class WireframeRenderer
{
public:
    WireframeRenderer();
    ~WireframeRenderer();

    bool init(ID3D11Device* device);
    void set(ID3D11DeviceContext* context);
    void toggle();
    void release();

private:
    ID3D11RasterizerState* m_wireframe_state = nullptr;
    ID3D11RasterizerState* m_solid_state = nullptr;

    bool m_is_wireframe = true; //Initially makes it wireframe
};
