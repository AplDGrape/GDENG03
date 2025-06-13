#include "WireframeRenderer.h"
#include <iostream>

WireframeRenderer::WireframeRenderer()
{
}

WireframeRenderer::~WireframeRenderer()
{
    release();
}

bool WireframeRenderer::init(ID3D11Device* device)
{
    //Wireframe state
    D3D11_RASTERIZER_DESC wireframe_desc = {};
    /*D3D11_RASTERIZER_DESC wireframe_desc;
    ZeroMemory(&wireframe_desc, sizeof(D3D11_RASTERIZER_DESC));*/
    wireframe_desc.FillMode = D3D11_FILL_WIREFRAME;
    wireframe_desc.CullMode = D3D11_CULL_NONE;
    wireframe_desc.DepthClipEnable = TRUE;

    if (FAILED(device->CreateRasterizerState(&wireframe_desc, &m_wireframe_state)))
        return false;

    //Solid state
    D3D11_RASTERIZER_DESC solid_desc = {};
    solid_desc.FillMode = D3D11_FILL_SOLID;
    solid_desc.CullMode = D3D11_CULL_BACK;
    solid_desc.DepthClipEnable = TRUE;

    if (FAILED(device->CreateRasterizerState(&solid_desc, &m_solid_state)))
        return false;

    return true;
}

void WireframeRenderer::set(ID3D11DeviceContext* context)
{
    //context->RSSetState(m_is_wireframe ? m_wireframe_state : m_solid_state);

    if (m_is_wireframe)
    {
        //context->RSSetState(m_wireframe_state);
        context->RSSetState(m_solid_state);
    }
    else
    {
        context->RSSetState(m_wireframe_state);
        //context->RSSetState(m_solid_state);
    }
}

void WireframeRenderer::toggle()
{
    m_is_wireframe = !m_is_wireframe;

    //Added Log status
    if (m_is_wireframe)
        std::cout << "[Render Mode] Wireframe mode ON\n";
    else
        std::cout << "[Render Mode] Solid mode ON\n";
}

void WireframeRenderer::release()
{
    if (m_wireframe_state)
    {
        m_wireframe_state->Release();
        m_wireframe_state = nullptr;
    }
    if (m_solid_state)
    {
        m_solid_state->Release();
        m_solid_state = nullptr;
    }
}
