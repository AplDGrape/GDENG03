#include "DefaultShaderManager.h"
#include "GraphicsEngine.h"

VertexShader* DefaultShaderManager::defaultVS = nullptr;
PixelShader* DefaultShaderManager::defaultPS = nullptr;

void DefaultShaderManager::initialize()
{
    void* shader_byte_code = nullptr;
    size_t size_shader = 0;

    // Load vertex shader
    GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);
    defaultVS = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

    // Store shader bytecode for future use if needed
    GraphicsEngine::get()->releaseCompiledShader();

    // Load pixel shader
    GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
    defaultPS = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

    GraphicsEngine::get()->releaseCompiledShader();
}

void DefaultShaderManager::destroy()
{
    if (defaultVS) {
        defaultVS->release();
        defaultVS = nullptr;
    }

    if (defaultPS) {
        defaultPS->release();
        defaultPS = nullptr;
    }
}

VertexShader* DefaultShaderManager::getVertexShader()
{
    return defaultVS;
}

PixelShader* DefaultShaderManager::getPixelShader()
{
    return defaultPS;
}
