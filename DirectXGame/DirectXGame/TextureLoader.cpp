#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"
#include "external/stb_image.h"
#include <iostream>

ID3D11ShaderResourceView* TextureLoader::LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filename)
{
    int width, height, channels;
    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha); // Force RGBA
    if (!imageData)
    {
        std::cout << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }

    // Describe the texture
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = imageData;
    initData.SysMemPitch = width * 4;

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &initData, &texture);
    stbi_image_free(imageData);

    if (FAILED(hr))
    {
        std::cout << "Failed to create texture from image: " << filename << std::endl;
        return nullptr;
    }

    // Create shader resource view
    ID3D11ShaderResourceView* srv = nullptr;
    hr = device->CreateShaderResourceView(texture, nullptr, &srv);
    texture->Release();

    if (FAILED(hr))
    {
        std::cout << "Failed to create SRV from texture: " << filename << std::endl;
        return nullptr;
    }

    return srv;
}