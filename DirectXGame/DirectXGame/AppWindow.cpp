#include "AppWindow.h"
#include "RenderMultipleQuad.h"
#include "WireframeRenderer.h"
#include "EngineTime.h"
#include <iostream>
#include <Windows.h>

//struct vec3
//{
//	float x, y, z;
//};
//
//struct vertex
//{
//	vec3 position;
//	vec3 color;
//};

__declspec(align(16))
struct constant
{
	unsigned int m_time;
};

AppWindow::AppWindow()
{
}

void AppWindow::onCreate()
{
	Window::onCreate();
	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	RenderMultipleQuad::initialize();

	vertex list[] =
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
		{-0.5f, -0.5f, 0.0f,	-0.32f,-0.11f, 0.0f,	0,1,0,	1,0,0}, //POS1

		{-0.5f,  0.5f, 0.0f,	-0.11f, 0.78f, 0.0f,	0,1,0,	1,1,0}, //POS2

		{ 0.5f, -0.5f, 0.0f,	 0.75f,-0.73f, 0.0f,	0,1,0,	1,0,1},  //POS3

		{ 0.5f,  0.5f, 0.0f,	 0.88f, 0.77f, 0.0f,	0,1,0,	1,1,1} //POS4
	};

	//const vertex* list = RenderMultipleQuad::getInstance()->getVertexList();
	//size_t size_list = RenderMultipleQuad::getInstance()->getVertexCount();

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);
	//Bring this back for animate
	
	//GraphicsEngine::get()->createShaders();
	
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	
	//Vertex Shader
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getShaderBufferAndSize(&shader_byte_code, &size_shader);
	
	//m_vb->load((void*)list, sizeof(vertex), (UINT)size_list, shader_byte_code, size_shader);
	m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	//Pixel Shader
	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);

	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getShaderBufferAndSize(&shader_byte_code, &size_shader);

	//m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	m_cb = GraphicsEngine::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));

	//wireframe
	m_wireframe_renderer = new WireframeRenderer();
	if (m_wireframe_renderer->init(GraphicsEngine::get()->getD3DDevice()))
	{
		// Handle error / incase some shit happens
	}

}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	//CLEAR THE RENDER TARGET
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.0, 0.0, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	//SET THE DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	//GraphicsEngine::get()->setShaders();

	// Check if "-" key is pressed, decrease time by 1 second
	if (GetAsyncKeyState('-') & 0x1)
	{
		EngineTime::decreaseTime(1.0);
	}

	// Check if "=" key (plus key) is pressed, increase time by 1 second
	if (GetAsyncKeyState('=') & 0x1)
	{
		EngineTime::increaseTime(1.0);
	}

	constant cc;
	cc.m_time = ::GetTickCount();
	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	//Wireframe
	if (GetAsyncKeyState('T') & 0x1) //Press T to toggle
	{
		m_wireframe_renderer->toggle(); //Toggles with this line
	}

	m_wireframe_renderer->set(GraphicsEngine::get()->getD3DDeviceContext());

	// Check if "T" key is pressed to render triangle
	if (GetAsyncKeyState('E') & 0x8000)
	{
		RenderMultipleQuad::getInstance()->setRenderShape(false); // Set to triangle
	}

	// Check if "Q" key is pressed to render quad
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		RenderMultipleQuad::getInstance()->setRenderShape(true); // Set to quad
	}

	//FINALLY DRAW THE TRIANGLE
	//For animate part
	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	//GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleList(m_vb->getSizeVertexList(), 0);

	m_swap_chain->present(false);

	//int x = RenderMultipleQuad::getInstance()->getX();
	//std::cout << "My X " << x << std::endl;
	
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();

	RenderMultipleQuad::destroy();

	//wireframe
	m_wireframe_renderer->release();
	delete m_wireframe_renderer;
	m_wireframe_renderer = nullptr;

	GraphicsEngine::get()->release();
}

AppWindow::~AppWindow()
{
}