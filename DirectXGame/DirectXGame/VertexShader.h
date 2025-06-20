#pragma once
#include <d3d11.h>

class GraphicsEngine;
class DeviceContext;

class VertexShader
{
public:
	VertexShader();
	void release();
	~VertexShader();
	ID3D11VertexShader* getShader() const;
	ID3D11InputLayout* getInputLayout() const;

	void getShaderBufferAndSize(void** byte_code, size_t* size);

private:
	bool init(const void* shader_byte_code, size_t byte_code_size);
private:
	ID3D11VertexShader* m_vs;
	ID3D11InputLayout* m_inputLayout;
	void* m_shader_byte_code = nullptr;
	size_t m_size_shader = 0;
private:
	friend class GraphicsEngine;
	friend class DeviceContext;
};

