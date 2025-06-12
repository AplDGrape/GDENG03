#pragma once
#include "Cube.h"
#include "AGameObject.h"

class Plane : public AGameObject
{
public:
	Plane(string name, void* shaderByteCode, size_t sizeShader);
	~Plane();

	void update(float deltaTime) override;
	void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;

private:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	ConstantBuffer* constantBuffer;

	float deltaTime = 0;
};

