#pragma once
#include <d3d11.h>
#include <string>

class TextureLoader
{
public:
    static ID3D11ShaderResourceView* LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filename);
};
