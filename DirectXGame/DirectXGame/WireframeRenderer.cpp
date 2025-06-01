#include "WireframeRenderer.h"

WireframeRenderer::WireframeRenderer()
{
}

WireframeRenderer::~WireframeRenderer()
{
    release();
}

bool WireframeRenderer::init(ID3D11Device* device)
{
    D3D11_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D11_FILL_WIREFRAME; // Key part
    desc.CullMode = D3D11_CULL_BACK;
    desc.DepthClipEnable = TRUE;

    HRESULT hr = device->CreateRasterizerState(&desc, &m_wireframe_state);
    return SUCCEEDED(hr);
}

void WireframeRenderer::set(ID3D11DeviceContext* context)
{
    context->RSSetState(m_wireframe_state);
}

void WireframeRenderer::release()
{
    if (m_wireframe_state)
    {
        m_wireframe_state->Release();
        m_wireframe_state = nullptr;
    }
}
