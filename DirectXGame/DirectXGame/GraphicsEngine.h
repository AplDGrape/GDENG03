#pragma once

#include <d3d11.h>
class SwapChain;
class DeviceContext;
class VertexBuffer;
class VertexShader;

class GraphicsEngine
{
public:
	GraphicsEngine();
	//Initialize the GraphicsEngine and DirectX 11 Device
	bool init();

	bool compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	void releaseCompiledShader();

	//DEFAULT SIMPLE SHADERS
	bool createShaders();
	bool setShaders();
	//void getShaderBufferAndSize(void** bytecode, UINT* size);

	//Release all the resources loaded
	bool release();
	~GraphicsEngine();
public:
	SwapChain* createSwapChain();
	DeviceContext* getImmediateDeviceContext();
	VertexBuffer* createVertexBuffer();

	VertexShader* createVertexShader(const void* shader_byte_code, size_t byte_code_size);
public:
	static GraphicsEngine* get();

private:
	DeviceContext* m_imm_device_context;

private:
	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level;

	ID3D11DeviceContext* m_imm_context;
private:
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
private:
	ID3DBlob* m_blob = nullptr;

	ID3DBlob* m_vsblob = nullptr;
	ID3DBlob* m_psblob = nullptr;
	ID3D11VertexShader* m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;
private:
	friend class SwapChain;
	friend class VertexBuffer;
	friend class VertexShader;
};

