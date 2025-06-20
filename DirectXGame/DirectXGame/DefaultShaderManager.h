#pragma once
#include "VertexShader.h"
#include "PixelShader.h"

class DefaultShaderManager
{
public:
    static void initialize();
    static void destroy();

    static VertexShader* getVertexShader();
    static PixelShader* getPixelShader();

private:
    static VertexShader* defaultVS;
    static PixelShader* defaultPS;
};
