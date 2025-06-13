#include "AppWindow.h"
#include "RenderMultipleQuad.h"
#include "Cube.h"
#include "Plane.h"
#include "WireframeRenderer.h"
#include "EngineTime.h"
#include "Vector3D.h"
#include "Matrix4x4.h"
#include <iostream>
#include <Windows.h>
#include "InputSystem.h"

#include "MathUtils.h"
#include "CubeMeshData.h"

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

struct InstanceData
{
	Matrix4x4 transform;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
};

AppWindow::AppWindow()
{
}

void AppWindow::update()
{
	constant cc;
	cc.m_time = ::GetTickCount();
	//cc.m_time = static_cast<unsigned int>(EngineTime::getTime() * 1000); // milliseconds
	
	m_delta_pos += m_delta_time / 4.0f;

	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;

	Matrix4x4 temp;

	//cc.m_world.setTranslation(Vector3D::lerp(Vector3D(-2, -2, 0), Vector3D(2, 2, 0), m_delta_pos));
	
	m_delta_scale += m_delta_time / 0.5f;
	
	//cc.m_world.setScale(Vector3D::lerp(Vector3D(0.5, 0.5, 0), Vector3D(1.0f, 1.0f, 0), (sin(m_delta_scale)+1.0f)/2.0f));

	//temp.setTranslation(Vector3D::lerp(Vector3D(-1.5f, -1.5f, 0), Vector3D(1.5f, 1.5f, 0), m_delta_pos));

	//cc.m_world *= temp;

	/*cc.m_world.setScale(Vector3D(m_scale_cube, m_scale_cube, m_scale_cube));

	temp.setIdentity();
	temp.setRotationZ(0.0f);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	cc.m_world *= temp;*/

	cc.m_world.setIdentity();

	Matrix4x4 world_cam;
	world_cam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	world_cam *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	world_cam *= temp;

	Vector3D new_pos = m_world_cam.getTranslation() + world_cam.getZDirection() * (m_forward * 0.3f);

	new_pos = new_pos + world_cam.getXDirection() * (m_rightward * 0.3f);

	world_cam.setTranslation(new_pos);

	m_world_cam = world_cam;

	world_cam.getInverse();

	cc.m_view = world_cam;
	/*cc.m_proj.setOrthoLH
	(
		(this->getClientWindowRect().right - this->getClientWindowRect().left)/400.0f,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top)/400.0f,
		-4.0f,
		4.0f
	);*/
	
	int POVwidth = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int POVheight = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	cc.m_proj.setPerspectiveFovLH(1.57f, ((float)POVwidth / (float)POVheight), 0.1f, 100.0f);

	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);


}

void AppWindow::onCreate()
{
	Window::onCreate();

	InputSystem::get()->addListener(this);
	InputSystem::get()->showCursor(false);

	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	RenderMultipleQuad::initialize();
	EngineTime::initialize();

	m_world_cam.setTranslation(Vector3D(0, 0, -2));

	vertex vertex_list[] =
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
		{Vector3D (-0.5f, -0.5f, -0.5f),	/*Vector3D (-0.32f,-0.11f, 0.0f),*/	Vector3D (1,0,0),	Vector3D(0.2f,0,0)}, //POS1

		{Vector3D (-0.5f,  0.5f, -0.5f),	/*Vector3D (-0.11f, 0.78f, 0.0f),*/	Vector3D (1,1,0),	Vector3D(0.2f,0.2f,0)}, //POS2

		{Vector3D ( 0.5f,  0.5f, -0.5f),	/*Vector3D (0.75f,-0.73f, 0.0f),*/	Vector3D (1,1,0),	Vector3D(0.2f,0.2f,0)},  //POS3

		{Vector3D ( 0.5f, -0.5f, -0.5f),	/*Vector3D (0.88f, 0.77f, 0.0f),*/	Vector3D (1,0,0),	Vector3D(0.2f,0,0)}, //POS4

		//BACK FACE
		{Vector3D( 0.5f, -0.5f,  0.5f),	/*Vector3D (-0.32f,-0.11f, 0.0f),*/	Vector3D(0,1,0),	Vector3D(0,0.2f,0)}, //POS1

		{Vector3D( 0.5f,  0.5f,  0.5f),	/*Vector3D (-0.11f, 0.78f, 0.0f),*/	Vector3D(0,1,1),	Vector3D(0,0.2f,0.2f)}, //POS2

		{Vector3D(-0.5f,  0.5f,  0.5f),	/*Vector3D (0.75f,-0.73f, 0.0f),*/	Vector3D(0,1,1),	Vector3D(0,0.2f,0.2f)},  //POS3

		{Vector3D(-0.5f, -0.5f,  0.5f),	/*Vector3D (0.88f, 0.77f, 0.0f),*/	Vector3D(0,1,0),	Vector3D(0,0.2f,0)} //POS4
	};

	//const vertex* list = RenderMultipleQuad::getInstance()->getVertexList();
	//size_t size_list = RenderMultipleQuad::getInstance()->getVertexCount();

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(vertex_list);
	//Bring this back for animate
	
	//GraphicsEngine::get()->createShaders();
	
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

	m_ib = GraphicsEngine::get()->createIndexBuffer();
	UINT size_index_list = ARRAYSIZE(index_list);

	m_ib->load(index_list, size_index_list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	
	//Vertex Shader
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getShaderBufferAndSize(&shader_byte_code, &size_shader);
	
	//m_vb->load((void*)list, sizeof(vertex), (UINT)size_list, shader_byte_code, size_shader);
	m_vb->load(vertex_list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	//Pixel Shader
	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);

	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
	//GraphicsEngine::get()->getShaderBufferAndSize(&shader_byte_code, &size_shader);

	//m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	//Render multiple cube (i.e. 100)
	Cube* cubeObject = new Cube("Cube", shader_byte_code, size_shader);

	for (int i = 0; i < 100; i++) {
		float x = MathUtils::randomFloat(-0.75, 0.75f);
		float y = MathUtils::randomFloat(-0.75, 0.75f);
		float z = MathUtils::randomFloat(-0.75f, 0.75f); //For depth

		Cube* cubeObject = new Cube("Cube", shader_byte_code, size_shader);
		cubeObject->setAnimSpeed(MathUtils::randomFloat(-3.75f, 3.75f));
		cubeObject->setPosition(Vector3D(x, y, z/*0.0f*/));
		cubeObject->setScale(Vector3D(0.25, 0.25, 0.25));
		this->cubeList.push_back(cubeObject);
	}

	// Added temporary plane
	//Plane* plane = new Plane("MyPlane", shader_byte_code, size_shader);
	//plane->setPosition(Vector3D(0, -0.51f, 0.1f));
	//plane->setScale(Vector3D(5.0f, 1.0f, 5.0f));
	////plane->setRotation(Vector3D(0, 0, 0)); // Lay flat rotation
	//this->cubeList2.push_back(plane); // reusing cubeList for general drawables

	//VertexBuffer* m_instanceBuffer = nullptr;
	//m_instanceBuffer = GraphicsEngine::get()->createVertexBuffer();

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
	

	GraphicsEngine::get()->releaseCompiledShader();
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	EngineTime::update();

	InputSystem::get()->update();

	//CLEAR THE RENDER TARGET
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.0, 0.0, 1);
	//SET VIEWPORT OF RENDER TARGET IN WHICH WE HAVE TO DRAW
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	//SET THE DEFAULT SHADER IN THE GRAPHICS PIPELINE TO BE ABLE TO DRAW
	//GraphicsEngine::get()->setShaders();

	//constant cc;
	//cc.m_time = ::GetTickCount();
	////cc.m_time = static_cast<unsigned int>(EngineTime::getTime() * 1000); // milliseconds
	//m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);

	update();



	// Check if "-" key is pressed, decrease time by 1 second
	if (GetAsyncKeyState(VK_OEM_MINUS) & 0x1)
	{
		EngineTime::decreaseTime(1.0);
	}

	// Check if "=" key (plus key) is pressed, increase time by 1 second
	if (GetAsyncKeyState(VK_OEM_PLUS) & 0x1)
	{
		EngineTime::increaseTime(1.0);
	}

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);

	//SET THE VERTICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
	//GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffers(m_vb, m_instanceBuffer);
	//SET THE INDICES OF THE TRIANGLE TO DRAW
	GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(m_ib);

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
	//GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);
	//GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleList(m_vb->getSizeVertexList(), 0);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	int renderedCount = 0;

	////100 cubes
	//for (int i = 0; i < this->cubeList.size(); i++) {
	//	this->cubeList[i]->update(EngineTime::getDeltaTime());
	//	this->cubeList[i]->draw(width, height, m_vs, m_ps);
	//	renderedCount++;
	//}

	//// Render plane objects
	//for (int i = 0; i < this->cubeList2.size(); i++) {
	//	this->cubeList2[i]->update(EngineTime::getDeltaTime());
	//	this->cubeList2[i]->draw(width, height, m_vs, m_ps);
	//}

	m_swap_chain->present(true);

	//m_swap_chain->present(false);

	//int x = RenderMultipleQuad::getInstance()->getX();
	//std::cout << "My X " << x << std::endl;
	
	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount();

	m_delta_time = (m_old_delta)?((m_new_delta - m_old_delta) / 1000.0f):0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_ib->release();
	m_cb->release();
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

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'W')
	{
		//m_rot_x += 0.707f * m_delta_time;
		m_forward = 0.5f;
	}
	else if (key == 'S')
	{
		//m_rot_x -= 0.707f * m_delta_time;
		m_forward = -0.5f;
	}
	else if (key == 'A')
	{
		//m_rot_y += 0.707f * m_delta_time;
		m_rightward = -0.5f;
	}
	else if (key == 'D')
	{
		//m_rot_y -= 0.707f * m_delta_time;
		m_rightward = 0.5f;
	}
	else if (key == VK_ESCAPE)
	{
		//Sends a close message to the window
		PostMessage(this->m_hwnd, WM_CLOSE, 0, 0);
	}
}

void AppWindow::onKeyUp(int key)
{
	m_forward = 0.0f;
	m_rightward = 0.0f;
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	int POVwidth = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int POVheight = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_rot_x += (mouse_pos.m_y - (POVheight / 2.0f)) * m_delta_time * 0.1f;
	m_rot_y += (mouse_pos.m_x - (POVwidth / 2.0f))* m_delta_time * 0.1f;

	InputSystem::get()->setCursorPosition(Point(POVwidth / 2.0f, POVheight / 2.0f));
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 0.5f;
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	m_scale_cube = 2.0f;
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
	m_scale_cube = 1.0f;
}

AppWindow::~AppWindow()
{
}