#include "VertexShader.h"
#include "GraphicsEngine.h"

VertexShader::VertexShader()
{
}

void VertexShader::release()
{
    if (m_shader_byte_code)
    {
        free(m_shader_byte_code);
        m_shader_byte_code = nullptr;
        m_size_shader = 0;
    }

    m_vs->Release();
    delete this;
}

bool VertexShader::init(const void* shader_byte_code, size_t byte_code_size)
{
   /* if (!SUCCEEDED(GraphicsEngine::get()->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &m_vs)))
        return false;

    return true;*/
    ID3D11Device* device = GraphicsEngine::get()->m_d3d_device;

    // Create vertex shader
    if (FAILED(device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &m_vs)))
        return false;

    // Define input layout to match your Vertex struct and HLSL
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    if (FAILED(device->CreateInputLayout(layout, numElements, shader_byte_code, byte_code_size, &m_inputLayout)))
        return false;

    return true;
}

VertexShader::~VertexShader()
{
}

ID3D11VertexShader* VertexShader::getShader() const
{
    return m_vs;
}

ID3D11InputLayout* VertexShader::getInputLayout() const
{
    return m_inputLayout;
}

void VertexShader::getShaderBufferAndSize(void** byte_code, size_t* size)
{
    *byte_code = this->m_shader_byte_code;
    *size = this->m_size_shader;
}


