#include "Cube.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
//#include "CubeMeshData.h"

Cube::Cube(string name, void* shaderByteCode, size_t sizeShader):AGameObject(name)
{
	Vertex quadlist[] =
	{
		//X - Y - Z
		//RECT PRIDE
		//{-0.5f, -0.5f, 0.0f,	1,0,0}, //POS1

		//{-0.5f,  0.5f, 0.0f,	0,1,0}, //POS2

		//{ 0.5f, -0.5f, 0.0f,	0,0,1},  //POS3

		//{ 0.5f,  0.5f, 0.0f,	1,1,0} //POS4

		//TRIANGLE PRIDE
		//{-0.5f, -0.5f, 0.0f,	1,0,0}, //POS1

		//{ 0.0f,  0.5f, 0.0f,	0,1,0},	//POS2

		//{ 0.5f, -0.5f, 0.0f,	0,0,1}	//POS3

		//RECT GREEN
		//FRONT FACE
		{Vector3D(-0.5f, -0.5f, -0.5f),	/*Vector3D (-0.32f,-0.11f, 0.0f),*/	Vector3D(1,0,0),	Vector3D(0.2f,0,0)}, //POS1

		{Vector3D(-0.5f,  0.5f, -0.5f),	/*Vector3D (-0.11f, 0.78f, 0.0f),*/	Vector3D(1,1,0),	Vector3D(0.2f,0.2f,0)}, //POS2

		{Vector3D(0.5f,  0.5f, -0.5f),	/*Vector3D (0.75f,-0.73f, 0.0f),*/	Vector3D(1,1,0),	Vector3D(0.2f,0.2f,0)},  //POS3

		{Vector3D(0.5f, -0.5f, -0.5f),	/*Vector3D (0.88f, 0.77f, 0.0f),*/	Vector3D(1,0,0),	Vector3D(0.2f,0,0)}, //POS4

		//BACK FACE
		{Vector3D(0.5f, -0.5f,  0.5f),	/*Vector3D (-0.32f,-0.11f, 0.0f),*/	Vector3D(0,1,0),	Vector3D(0,0.2f,0)}, //POS1

		{Vector3D(0.5f,  0.5f,  0.5f),	/*Vector3D (-0.11f, 0.78f, 0.0f),*/	Vector3D(0,1,1),	Vector3D(0,0.2f,0.2f)}, //POS2

		{Vector3D(-0.5f,  0.5f,  0.5f),	/*Vector3D (0.75f,-0.73f, 0.0f),*/	Vector3D(0,1,1),	Vector3D(0,0.2f,0.2f)},  //POS3

		{Vector3D(-0.5f, -0.5f,  0.5f),	/*Vector3D (0.88f, 0.77f, 0.0f),*/	Vector3D(0,1,0),	Vector3D(0,0.2f,0)} //POS4
	};

	this->vertexBuffer = GraphicsEngine::get()->createVertexBuffer();
	this->vertexBuffer->load(quadlist, sizeof(Vertex), ARRAYSIZE(quadlist), shaderByteCode, sizeShader);

	unsigned int index_list[] =
	{
		//FRONT SIDE
		0,1,2,	//FIRST TRAINGLE
		2,3,0,	//SECOND TRIANGLE
		//BACK SIDE
		4,5,6,
		6,7,4,
		//TOP SIDE
		1,6,5,
		5,2,1,
		//BOTTOM SIDE
		7,0,3,
		3,4,7,
		//RIGHT SIDE
		3,2,5,
		5,4,3,
		//LEFT SIDE
		7,6,1,
		1,0,7
	};
	this->indexBuffer = GraphicsEngine::get()->createIndexBuffer();
	this->indexBuffer->load(index_list, ARRAYSIZE(index_list));

	//create constant buffer
	CBData cbData = {};
	cbData.time = 0;
	this->constantBuffer = GraphicsEngine::get()->createConstantBuffer();
	this->constantBuffer->load(&cbData, sizeof(CBData));
}

Cube::~Cube()
{
	this->vertexBuffer->release();
	this->indexBuffer->release();
	//AGameObject::~AGameObject();
}

void Cube::update(float deltaTime)
{
	this->deltaTime = deltaTime;

}

//void Cube::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
//{
//	GraphicsEngine* graphEngine = GraphicsEngine::get();
//	DeviceContext* deviceContext = graphEngine->getImmediateDeviceContext();
//
//	CBData cbData = {};
//
//	if (this->deltaPos > 1.0f)
//	{
//		this->deltaPos = 0.0f;
//	}
//	else {
//		this->deltaPos += this->deltaTime * 0.1f;
//	}
//
//	Matrix4x4 allMatrix; allMatrix.setIdentity();
//	Matrix4x4 translationMatrix; translationMatrix.setTranslation(this->getLocalPosition());
//	Matrix4x4 scaleMatrix; scaleMatrix.setScale(this->getLocalScale());
//	Vector3D rotation = this->getLocalRotation();
//	/*Matrix4x4 zMatrix; zMatrix.setRotationZ(rotation.getValues().z);
//	Matrix4x4 xMatrix; xMatrix.setRotationX(rotation.getValues().x);
//	Matrix4x4 yMatrix; yMatrix.setRotationY(rotation.getValues().y);*/
//	Matrix4x4 zMatrix; zMatrix.setRotationZ(rotation.m_z);
//	Matrix4x4 xMatrix; xMatrix.setRotationX(rotation.m_x);
//	Matrix4x4 yMatrix; yMatrix.setRotationY(rotation.m_y);
//
//	Matrix4x4 rotMatrix; rotMatrix.setIdentity();
//	rotMatrix = rotMatrix.multiplyTo(xMatrix.multiplyTo(yMatrix.multiplyTo(zMatrix)));
//	allMatrix = allMatrix.multiplyTo(scaleMatrix.multiplyTo(rotMatrix));
//	allMatrix = allMatrix.multiplyTo(translationMatrix);
//
//	cbData.worldMatrix = allMatrix;
//
//	cbData.viewMatrix.setIdentity();
//	cbData.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -4.0f, 4.0f);
//
//	this->constantBuffer->update(deviceContext, &cbData);
//	deviceContext->setConstantBuffer(vertexShader, this->constantBuffer);
//	deviceContext->setConstantBuffer(pixelShader, this->constantBuffer);
//
//	deviceContext->setIndexBuffer(this->indexBuffer);
//	deviceContext->setVertexBuffer(this->vertexBuffer);
//
//	deviceContext->drawIndexedTriangleList(this->indexBuffer->getSizeIndexList(), 0, 0);
//}

void Cube::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
	// Calculate half-size of orthographic projection
	float viewHalfWidth = width / 800.0f;  // since width use 400.0f
	float viewHalfHeight = height / 800.0f;

	Vector3D pos = this->getLocalPosition();
	Vector3D scale = this->getLocalScale();

	// AABB check for 2D ortho view (ignoring Z depth for now)
	if (pos.m_x + scale.m_x < -viewHalfWidth || pos.m_x - scale.m_x > viewHalfWidth ||
		pos.m_y + scale.m_y < -viewHalfHeight || pos.m_y - scale.m_y > viewHalfHeight)
	{
		// Skip rendering this cube
		return;
	}

	//existing code to draw the cube
	GraphicsEngine* graphEngine = GraphicsEngine::get();
	DeviceContext* deviceContext = graphEngine->getImmediateDeviceContext();

	CBData cbData = {};

	if (this->deltaPos > 1.0f)
		this->deltaPos = 0.0f;
	else
		this->deltaPos += this->deltaTime * 0.1f;

	Matrix4x4 allMatrix; allMatrix.setIdentity();
	Matrix4x4 translationMatrix; translationMatrix.setTranslation(pos);
	Matrix4x4 scaleMatrix; scaleMatrix.setScale(scale);
	Vector3D rotation = this->getLocalRotation();
	Matrix4x4 zMatrix; zMatrix.setRotationZ(rotation.m_z);
	Matrix4x4 xMatrix; xMatrix.setRotationX(rotation.m_x);
	Matrix4x4 yMatrix; yMatrix.setRotationY(rotation.m_y);
	Matrix4x4 rotMatrix; rotMatrix.setIdentity();
	rotMatrix = rotMatrix.multiplyTo(xMatrix.multiplyTo(yMatrix.multiplyTo(zMatrix)));
	allMatrix = allMatrix.multiplyTo(scaleMatrix.multiplyTo(rotMatrix));
	allMatrix = allMatrix.multiplyTo(translationMatrix);

	cbData.worldMatrix = allMatrix;
	cbData.viewMatrix.setIdentity();
	cbData.projMatrix.setOrthoLH(width / 400.0f, height / 400.0f, -4.0f, 4.0f);

	this->constantBuffer->update(deviceContext, &cbData);
	deviceContext->setConstantBuffer(vertexShader, this->constantBuffer);
	deviceContext->setConstantBuffer(pixelShader, this->constantBuffer);

	deviceContext->setIndexBuffer(this->indexBuffer);
	deviceContext->setVertexBuffer(this->vertexBuffer);
	deviceContext->drawIndexedTriangleList(this->indexBuffer->getSizeIndexList(), 0, 0);
}


void Cube::setAnimSpeed(float speed)
{
	this->speed = speed;
}
