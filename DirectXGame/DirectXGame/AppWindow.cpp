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

#include "SimplePlane.h"

//static ed::EditorContext* editorContext = nullptr;

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

enum class MathOpType
{
	Add,
	Subtract,
	Multiply,
	Divide,
	TransformCube, // Get transformation
	Print // The Print thingy
};

struct MathNode
{
	int id;
	MathOpType type;
	uint16_t inputA_id;
	uint16_t inputB_id;
	uint16_t output_id;

	/*uint16_t inputA = 0;
	uint16_t inputB = 0;
	uint16_t result = 0;*/
	int16_t inputA = 0;
	int16_t inputB = 0;
	int16_t result = 0;

	// For Transform Cube type
	Vector3D position;
	Vector3D rotation;
	Vector3D movement;
	bool applyToCube = false;

	// For the print function
	//std::string printMessage;
	char printMessage[128] = "Hello, Blueprint!";
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	//unsigned int m_time;
	float m_time = 0.0f;
	float padding[3] = { 0.0f, 0.0f, 0.0f };
};

AppWindow::AppWindow()
{
}

void DrawMathNode(MathNode& node);

void AppWindow::update()
{
	constant cc;
	cc.m_time = ::GetTickCount();
	//cc.m_time = static_cast<unsigned int>(EngineTime::getTime() * 1000); // milliseconds
	m_camera.update();

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
	cc.m_world.setScale(Vector3D(m_scale_cube, m_scale_cube, m_scale_cube));

	Vector3D blueprintPos(0, 0, 0);
	Vector3D blueprintRot(0, 0, 0);
	bool applyBlueprintTransform = false;

	// Find a transform node with applyToCube enabled
	for (auto& node : mathNodes)
	{
		if (node.type == MathOpType::TransformCube && node.applyToCube)
		{
			blueprintPos = node.position;
			blueprintRot = node.rotation;
			applyBlueprintTransform = true;
			break;
		}
	}

	if (applyBlueprintTransform)
	{
		cc.m_world.setIdentity();
		cc.m_world.setScale(Vector3D(m_scale_cube, m_scale_cube, m_scale_cube));

		// Apply rotation from blueprint
		temp.setIdentity();
		temp.setRotationZ(blueprintRot.m_z);
		cc.m_world *= temp;

		temp.setIdentity();
		temp.setRotationY(blueprintRot.m_y);
		cc.m_world *= temp;

		temp.setIdentity();
		temp.setRotationX(blueprintRot.m_x);
		cc.m_world *= temp;

		// Apply translation from blueprint
		temp.setIdentity();
		temp.setTranslation(blueprintPos);
		cc.m_world *= temp;
	}

	// Apply movement input to the camera
	m_camera.moveForward(m_forward);
	m_camera.strafeRight(m_rightward);

	// Use camera matrices
	cc.m_view = m_camera.getViewMatrix();

	/*cc.m_proj.setOrthoLH
	(
		(this->getClientWindowRect().right - this->getClientWindowRect().left)/400.0f,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top)/400.0f,
		-4.0f,
		4.0f
	);*/
	
	int POVwidth = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int POVheight = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	//cc.m_proj.setPerspectiveFovLH(1.57f, ((float)POVwidth / (float)POVheight), 0.1f, 100.0f);
	//Adjusted for zoom in and zoom out
	//cc.m_proj.setPerspectiveFovLH(1.57f - m_forward * 0.1f, ((float)POVwidth / (float)POVheight), 0.1f, 100.0f);

	m_camera.updateProjectionMatrix(m_camera.getFOV(), ((float)POVwidth / (float)POVheight), 0.1f, 100.0f);
	cc.m_proj = m_camera.getProjectionMatrix();

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

	float aspect = (float)(rc.right - rc.left) / (rc.bottom - rc.top);
	m_camera.updateProjectionMatrix(1.57f, aspect, 0.1f, 100.0f);

	RenderMultipleQuad::initialize();
	EngineTime::initialize();

	//IMGUI stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImGui::GetIO().FontGlobalScale = 0.5f;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(this->m_hwnd); // use your HWND
	ImGui_ImplDX11_Init(GraphicsEngine::get()->getD3DDevice(), GraphicsEngine::get()->getD3DDeviceContext());

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
	///*plane->setPosition(Vector3D(0, -0.51f, 0.1f));
	//plane->setScale(Vector3D(5.0f, 1.0f, 5.0f));*/
	////plane->setRotation(Vector3D(0, 0, 0)); // Lay flat rotation
	//this->cubeList2.push_back(plane);

	//VertexBuffer* m_instanceBuffer = nullptr;
	//m_instanceBuffer = GraphicsEngine::get()->createVertexBuffer();

	/*SimplePlane* plane = new SimplePlane("GroundPlane", shader_byte_code, size_shader);
	plane->setPosition(Vector3D(0, -0.51f, 0));
	plane->setScale(Vector3D(5.0f, 1.0f, 5.0f));
	this->cubeList2.push_back(plane);*/

	constant cc = {};
	//cc.m_time = 0;
	//m_cb->load(&cc, sizeof(constant));

	m_cb = GraphicsEngine::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));

	cc.m_time = ::GetTickCount();
	cc.m_view = m_camera.getViewMatrix();
	cc.m_proj = m_camera.getProjectionMatrix();
	cc.m_world.setIdentity();
	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);

	//wireframe
	m_wireframe_renderer = new WireframeRenderer();
	if (m_wireframe_renderer->init(GraphicsEngine::get()->getD3DDevice()))
	{
		// Handle error / incase some shit happens
	}
	

	GraphicsEngine::get()->releaseCompiledShader();
}

int GetNewPinID(std::vector<int>& freePinIds, int& nextId)
{
	if (!freePinIds.empty())
	{
		int id = freePinIds.back();
		freePinIds.pop_back();
		return id;
	}
	return nextId++;
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
	m_wireframe_renderer->set(GraphicsEngine::get()->getD3DDeviceContext());

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

	// Render plane objects
	for (int i = 0; i < this->cubeList2.size(); i++) {
		this->cubeList2[i]->update(EngineTime::getDeltaTime());
		this->cubeList2[i]->draw(width, height, m_vs, m_ps);
	}

	for (auto obj : this->cubeList2)
	{
		obj->update(EngineTime::getDeltaTime());
		obj->draw(width, height, m_vs, m_ps);
	}

	//Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Only allow UI interaction when mouse is visible
	if (m_mouseVisible)
	{		
		// Blueprint-style editor (imgui-node-editor)

		if (!editorContext)
		{
			ax::NodeEditor::Config config;
			config.SettingsFile = "BlueprintNodeEditor.json";
			editorContext = ax::NodeEditor::CreateEditor(&config);
		}

		ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Once);
		ImGui::Begin("Blueprint Editor");

		// Add node creation buttons
		if (ImGui::Button("Add Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.inputB_id = GetNewPinID(freePinIds, nextId);
			node.output_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Add;
			mathNodes.push_back(node);
		}
		ImGui::SameLine();
		if (ImGui::Button("Subtract Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.inputB_id = GetNewPinID(freePinIds, nextId);
			node.output_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Subtract;
			mathNodes.push_back(node);
		}
		ImGui::SameLine();
		if (ImGui::Button("Multiply Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.inputB_id = GetNewPinID(freePinIds, nextId);
			node.output_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Multiply;
			mathNodes.push_back(node);
		}
		ImGui::SameLine();
		if (ImGui::Button("Divide Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.inputB_id = GetNewPinID(freePinIds, nextId);
			node.output_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Divide;
			mathNodes.push_back(node);
		}
		if (ImGui::Button("Transform Cube Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.inputB_id = GetNewPinID(freePinIds, nextId);
			node.output_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::TransformCube;
			node.position = Vector3D(0, 0, 0);
			node.rotation = Vector3D(0, 0, 0);
			node.movement = Vector3D(0, 0, 0);
			node.applyToCube = false;
			mathNodes.push_back(node);
		}
		ImGui::SameLine();
		if (ImGui::Button("Print Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.output_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Print;
			strcpy_s(node.printMessage, "Hello from Blueprint!");
			mathNodes.push_back(node);
		}
		if (ImGui::Button("Delete Last Node") && !mathNodes.empty())
		{
			MathNode& node = mathNodes.back();
			freePinIds.push_back(node.id);
			freePinIds.push_back(node.inputA_id);
			freePinIds.push_back(node.inputB_id);
			freePinIds.push_back(node.output_id);
			mathNodes.pop_back();
		}

		// Start drawing editor
		ax::NodeEditor::SetCurrentEditor(editorContext);
		ax::NodeEditor::Begin("MyEditor", ImVec2(0.0f, 0.0f));

		for (auto& node : mathNodes)
		{
			DrawMathNode(node);
		}

		ax::NodeEditor::End();

		static bool firstTime = true;
		if (firstTime)
		{
			ax::NodeEditor::NavigateToContent();
			firstTime = false;
		}

		ImGui::End();
	}

	//printf("ImGui Version: %s\n", ImGui::GetVersion());

	//Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_swap_chain->present(true);

	//m_swap_chain->present(false);
	
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

	//IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	/*if (editorContext)
	{
		ax::NodeEditor::DestroyEditor(editorContext);
		editorContext = nullptr;
	}*/

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
	if (ImGui::GetIO().WantCaptureKeyboard)
		return; // Block game key handling while typing in ImGui

	if (key == 'M' && !m_mKeyDown)
	{
		m_mouseVisible = !m_mouseVisible;
		InputSystem::get()->showCursor(m_mouseVisible);
		m_mKeyDown = true;

		if (m_mouseVisible)
		{
			std::cout << "Mouse unlocked.\n";
		}
		else
		{
			std::cout << "Mouse locked.\n";
			// Reset cursor to center when locking again
			RECT rc = this->getClientWindowRect();
			InputSystem::get()->setCursorPosition(Point((rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2));
		}
	}
	if (key == 'T' && !m_tKeyDown)
	{
		m_wireframe_renderer->toggle(); // Toggles wireframe mode
		m_tKeyDown = true;
	}

	if (!m_mouseVisible)
	{
		if (key == 'W')
		{
			//m_forward = 0.05f;
			m_camera.moveForward(1.0f);
		}
		else if (key == 'S')
		{
			//m_forward = -0.05f;
			m_camera.moveForward(-1.0f);
		}
		else if (key == 'A')
		{
			//m_rightward = -0.05f;
			m_camera.strafeRight(1.0f);
		}
		else if (key == 'D')
		{
			//m_rightward = 0.05f;
			m_camera.strafeRight(-1.0f);
		}
		else if (key == 'Z')
		{
			m_camera.zoomIn(0.05f);
			if (!m_zKeyDown) // Only print once per press
			{
				std::cout << "Zoom in\n";
				m_zKeyDown = true;
			}
		}
		else if (key == 'C')
		{
			m_camera.zoomOut(0.05f);
			if (!m_cKeyDown) // Only print once per press
			{
				std::cout << "Zoom out\n";
				m_cKeyDown = true;
			}
		}
	}

	if (key == VK_ESCAPE)
	{
		//Sends a close message to the window
		PostMessage(this->m_hwnd, WM_CLOSE, 0, 0);
	}
}

void AppWindow::onKeyUp(int key)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
		return;

	if (key == 'M')
		m_mKeyDown = false;
	if (key == 'Z') 
		m_zKeyDown = false;
	if (key == 'C') 
		m_cKeyDown = false;
	if (key == 'T')
		m_tKeyDown = false;

	if (!m_mouseVisible)
	{
		m_forward = 0.0f;
		m_rightward = 0.0f;
	}
}

void AppWindow::onMouseMove(const Point& mouse_pos)
{
	if (m_mouseVisible)
		return;

	/*int POVwidth = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int POVheight = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	m_rot_x += (mouse_pos.m_y - (POVheight / 2.0f)) * m_delta_time * 0.1f;
	m_rot_y += (mouse_pos.m_x - (POVwidth / 2.0f))* m_delta_time * 0.1f;

	InputSystem::get()->setCursorPosition(Point(POVwidth / 2.0f, POVheight / 2.0f));*/

	int width = getClientWindowRect().right - getClientWindowRect().left;
	int height = getClientWindowRect().bottom - getClientWindowRect().top;

	float yawDelta = (mouse_pos.m_x - width / 2.0f) * m_delta_time * 0.1f;
	float pitchDelta = -(mouse_pos.m_y - height / 2.0f) * m_delta_time * 0.1f;

	m_camera.rotate(pitchDelta, yawDelta);

	InputSystem::get()->setCursorPosition(Point(width / 2, height / 2));
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	if (!m_mouseVisible)
		m_scale_cube = 0.5f;
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	if (!m_mouseVisible)
		m_scale_cube = 1.0f;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
	if (!m_mouseVisible)
		m_scale_cube = 2.0f;
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
	if (!m_mouseVisible)
		m_scale_cube = 1.0f;
}

void DrawMathNode(MathNode& node)
{
	ax::NodeEditor::BeginNode(node.id);

	// Handle Cube transformation, rotation, position
	if (node.type == MathOpType::TransformCube)
	{
		ImGui::Text("Transform Cube");

		ImGui::DragFloat3("Position", &node.position.m_x, 0.1f);
		ImGui::DragFloat3("Rotation", &node.rotation.m_x, 0.1f);
		ImGui::DragFloat3("Movement", &node.movement.m_x, 0.1f);
		ImGui::Checkbox("Apply", &node.applyToCube);

		ax::NodeEditor::EndNode();
		return;
	}

	// Handle Print function
	if (node.type == MathOpType::Print)
	{
		ImGui::Text("Print Node");

		ax::NodeEditor::BeginPin(node.inputA_id, ax::NodeEditor::PinKind::Input);
		ImGui::Text("In");
		ax::NodeEditor::EndPin();

		ImGui::InputText("Message", node.printMessage, IM_ARRAYSIZE(node.printMessage));
		ImGui::SameLine();
		if (ImGui::Button("Print"))
		{
			std::cout << "Blueprint Print: " << node.printMessage << std::endl;
		}

		ax::NodeEditor::EndNode();
		return;
	}

	// Handle math operation nodes (Add, Subtract, Multiply, Divide)
	std::string nodeLabel;
	switch (node.type)
	{
	case MathOpType::Add: nodeLabel = "Addition"; break;
	case MathOpType::Subtract: nodeLabel = "Subtraction"; break;
	case MathOpType::Multiply: nodeLabel = "Multiplication"; break;
	case MathOpType::Divide: nodeLabel = "Division"; break;
	}

	ImGui::PushID(node.id); // To get separate nodes
	ImGui::Text("%s", nodeLabel.c_str());

	ImGui::PushItemWidth(80.0f);

	ax::NodeEditor::BeginPin(node.inputA_id, ax::NodeEditor::PinKind::Input);
	ImGui::DragScalar("A", ImGuiDataType_U16, &node.inputA, 1.0f);
	ax::NodeEditor::EndPin();

	ax::NodeEditor::BeginPin(node.inputB_id, ax::NodeEditor::PinKind::Input);
	ImGui::DragScalar("B", ImGuiDataType_U16, &node.inputB, 1.0f);
	ax::NodeEditor::EndPin();

	ax::NodeEditor::BeginPin(node.output_id, ax::NodeEditor::PinKind::Output);
	switch (node.type)
	{
	case MathOpType::Add: node.result = node.inputA + node.inputB; break;
	case MathOpType::Subtract: node.result = node.inputA - node.inputB; break;
	case MathOpType::Multiply: node.result = node.inputA * node.inputB; break;
	case MathOpType::Divide: node.result = node.inputB != 0 ? node.inputA / node.inputB : 0; break;

	}

	//ImGui::Text("= %u", node.result); // %u for unsigned integer
	ImGui::Text("= %d", node.result);
	ax::NodeEditor::EndPin();

	ImGui::PopItemWidth();
	ImGui::PopID();

	ax::NodeEditor::EndNode();
}

AppWindow::~AppWindow()
{
}