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
	Print, // The Print thingy
	CubeNode // The Cube that will be transformed/changed etc.
};

struct MathNode
{
	int id;
	MathOpType type;
	uint16_t inputA_id;
	uint16_t inputB_id;

	uint16_t outputA_id;
	uint16_t outputB_id;
	uint16_t outputResult_id;

	int16_t inputA = 0;
	int16_t inputB = 0;
	int16_t result = 0;

	// The Base Cube Vectors
	Vector3D cubePosition;
	Vector3D cubeRotation;
	Vector3D cubeScale = Vector3D(1, 1, 1);

	// For Transform Cube type
	Vector3D position;
	Vector3D rotation;
	Vector3D scale = Vector3D(1, 1, 1);
	bool applyToCube = false;

	uint16_t outputPos_id;
	uint16_t outputRot_id;
	uint16_t outputScale_id;

	uint16_t inputPos_id;
	uint16_t inputRot_id;
	uint16_t inputScale_id;

	// For storing the resolved transform values in CubeNode
	Vector3D resolvedPos;
	Vector3D resolvedRot;
	Vector3D resolvedScale;

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
	editorContext = nullptr;
}

bool IsPinLinked(int pinId, const std::vector<Link>& links);
int16_t ResolveInputValue(int pinId, const std::vector<MathNode>& mathNodes, const std::vector<Link>& links, int16_t& outValue);
bool ResolveVectorInput(int pinId, const std::vector<MathNode>& mathNodes, const std::vector<Link>& links, Vector3D& outValue);
void DrawMathNode(MathNode& node, const std::vector<MathNode>& mathNodes, const std::vector<Link>& links);
void EvaluateMathNodes(std::vector<MathNode>& nodes, const std::vector<Link>& links);

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

	cc.m_world.setIdentity();
	//cc.m_world.setScale(Vector3D(m_scale_cube, m_scale_cube, m_scale_cube));

	Vector3D blueprintPos(0, 0, 0);
	Vector3D blueprintRot(0, 0, 0);
	Vector3D blueprintScale(1, 1, 1);
	bool applyBlueprintTransform = false;

	// Apply transforms into cube when linked
	for (auto& node : mathNodes)
	{
		if (node.type == MathOpType::CubeNode)
		{
			// Apply the resolved transform, whether linked or not
			blueprintPos = node.resolvedPos;
			blueprintRot = node.resolvedRot;
			blueprintScale = node.resolvedScale;
			applyBlueprintTransform = true;
			break;
		}
	}

	if (applyBlueprintTransform)
	{
		cc.m_world.setIdentity();

		// Apply scaling from blueprint
		cc.m_world.setScale(Vector3D(blueprintScale.m_x, blueprintScale.m_y, blueprintScale.m_z));

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
	
	int POVwidth = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int POVheight = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

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
			node.outputA_id = GetNewPinID(freePinIds, nextId);
			node.outputB_id = GetNewPinID(freePinIds, nextId);
			node.outputResult_id = GetNewPinID(freePinIds, nextId);
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
			node.outputA_id = GetNewPinID(freePinIds, nextId);
			node.outputB_id = GetNewPinID(freePinIds, nextId);
			node.outputResult_id = GetNewPinID(freePinIds, nextId);
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
			node.outputA_id = GetNewPinID(freePinIds, nextId);
			node.outputB_id = GetNewPinID(freePinIds, nextId);
			node.outputResult_id = GetNewPinID(freePinIds, nextId);
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
			node.outputA_id = GetNewPinID(freePinIds, nextId);
			node.outputB_id = GetNewPinID(freePinIds, nextId);
			node.outputResult_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Divide;
			mathNodes.push_back(node);
		}
		if (ImGui::Button("Cube Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputPos_id = GetNewPinID(freePinIds, nextId);
			node.inputRot_id = GetNewPinID(freePinIds, nextId);
			node.inputScale_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::CubeNode;
			node.cubePosition = Vector3D(0, 0, 0);
			node.cubeRotation = Vector3D(0, 0, 0);
			node.cubeScale = Vector3D(1, 1, 1);
			mathNodes.push_back(node);
		}
		ImGui::SameLine();
		if (ImGui::Button("Transform Cube Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.outputPos_id = GetNewPinID(freePinIds, nextId);
			node.outputRot_id = GetNewPinID(freePinIds, nextId);
			node.outputScale_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::TransformCube;
			node.position = Vector3D(0, 0, 0);
			node.rotation = Vector3D(0, 0, 0);
			node.scale = Vector3D(1, 1, 1);
			mathNodes.push_back(node);
		}
		ImGui::SameLine();
		if (ImGui::Button("Print Node"))
		{
			MathNode node;
			node.id = GetNewPinID(freePinIds, nextId);
			node.inputA_id = GetNewPinID(freePinIds, nextId);
			node.outputResult_id = GetNewPinID(freePinIds, nextId);
			node.type = MathOpType::Print;
			strcpy_s(node.printMessage, "Hello from Blueprint!");
			mathNodes.push_back(node);
		}
		if (ImGui::Button("Delete Last Node") && !mathNodes.empty())
		{
			MathNode& node = mathNodes.back();
			freePinIds.push_back(node.id);
			freePinIds.push_back(node.inputA_id);
			if (node.type != MathOpType::Print && node.type != MathOpType::TransformCube)
				freePinIds.push_back(node.inputB_id);
			freePinIds.push_back(node.outputResult_id);
			mathNodes.pop_back();
		}

		// Start drawing editor
		ax::NodeEditor::SetCurrentEditor(editorContext);
		ax::NodeEditor::Begin("MyEditor", ImVec2(0.0f, 0.0f));

		//EvaluateMathNodes(mathNodes, links);

		for (auto& node : mathNodes)
		{
			DrawMathNode(node, mathNodes, links);
		}

		// Draw links
		for (auto& link : links)
		{
			ax::NodeEditor::Link(link.id, link.startPinId, link.endPinId);
		}

		// Handle new links
		if (ax::NodeEditor::BeginCreate())
		{
			ax::NodeEditor::PinId pinA, pinB;
			if (ax::NodeEditor::QueryNewLink(&pinA, &pinB))
			{
				if (pinA && pinB)
				{
					int a = pinA.Get();
					int b = pinB.Get();

					// Find which pin is output and which is input
					auto isInputPin = [&](int pinId) -> bool {
						for (const MathNode& node : mathNodes)
						{
							if (node.inputA_id == pinId || node.inputB_id == pinId)
								return true;
						}
						return false;
					};

					int inputPin = isInputPin(a) ? a : b;
					int outputPin = (inputPin == a) ? b : a;

					// Prevent duplicate link
					bool alreadyLinked = false;
					for (const auto& link : links)
					{
						if (link.startPinId == outputPin && link.endPinId == inputPin)
						{
							alreadyLinked = true;
							break;
						}
					}

					if (!alreadyLinked)
					{
						// Only add the link if the user released the mouse button
						if (ax::NodeEditor::AcceptNewItem())
						{
							links.push_back({ nextLinkId++, outputPin, inputPin });
							std::cout << "Created Link from output pin " << outputPin << " to input pin " << inputPin << std::endl;
						}
					}
				}
			}
		}
		ax::NodeEditor::EndCreate();

		EvaluateMathNodes(mathNodes, links);

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

bool IsPinLinked(int pinId, const std::vector<Link>& links)
{
	for (const Link& link : links)
	{
		if (link.endPinId == pinId)
			return true;
	}
	return false;
}

void EvaluateMathNodes(std::vector<MathNode>& nodes, const std::vector<Link>& links)
{
	// Loop through all nodes and compute their result before drawing
	for (auto& node : nodes)
	{
		int16_t valA = node.inputA;
		int16_t valB = node.inputB;

		if (IsPinLinked(node.inputA_id, links))
		{
			ResolveInputValue(node.inputA_id, nodes, links, valA);		
		}
		if (IsPinLinked(node.inputB_id, links))
		{
			ResolveInputValue(node.inputB_id, nodes, links, valB);
		}

		node.inputA = valA;
		node.inputB = valB;

		switch (node.type)
		{
		case MathOpType::Add: node.result = valA + valB; break;
		case MathOpType::Subtract: node.result = valA - valB; break;
		case MathOpType::Multiply: node.result = valA * valB; break;
		case MathOpType::Divide: node.result = (valB != 0) ? valA / valB : 0; break;
		default: break;
		}

		// Cube handling
		if (node.type == MathOpType::CubeNode)
		{
			// Default to node's own transform unless linked
			node.resolvedPos = node.cubePosition;
			node.resolvedRot = node.cubeRotation;
			node.resolvedScale = node.cubeScale;

			ResolveVectorInput(node.inputPos_id, nodes, links, node.resolvedPos);
			ResolveVectorInput(node.inputRot_id, nodes, links, node.resolvedRot);
			ResolveVectorInput(node.inputScale_id, nodes, links, node.resolvedScale);
		}
	}
}

void DrawMathNode(MathNode& node, const std::vector<MathNode>& mathNodes, const std::vector<Link>& links)
{
	ax::NodeEditor::BeginNode(node.id);

	ImGui::PushID(node.id);

	// Handle Cube transformation, rotation, position
	if (node.type == MathOpType::TransformCube)
	{
		ImGui::Text("Transform Cube");

		// Show editable fields
		ImGui::DragFloat3("Position", &node.position.m_x, 0.1f);
		ImGui::DragFloat3("Rotation", &node.rotation.m_x, 0.1f);
		ImGui::DragFloat3("Scale", &node.scale.m_x, 0.1f);

		// Output pins
		ax::NodeEditor::BeginPin(node.outputPos_id, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out Pos");
		ax::NodeEditor::EndPin();

		ax::NodeEditor::BeginPin(node.outputRot_id, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out Rot");
		ax::NodeEditor::EndPin();

		ax::NodeEditor::BeginPin(node.outputScale_id, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out Scale");
		ax::NodeEditor::EndPin();

		ImGui::PopID();
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

		ImGui::PopID();
		ax::NodeEditor::EndNode();
		return;
	}

	// The Cube that will be handled
	if (node.type == MathOpType::CubeNode)
	{
		ImGui::Text("Cube Node");

		// Input pins
		ax::NodeEditor::BeginPin(node.inputPos_id, ax::NodeEditor::PinKind::Input);
		ImGui::Text("Pos In");
		ax::NodeEditor::EndPin();

		ax::NodeEditor::BeginPin(node.inputRot_id, ax::NodeEditor::PinKind::Input);
		ImGui::Text("Rot In");
		ax::NodeEditor::EndPin();

		ax::NodeEditor::BeginPin(node.inputScale_id, ax::NodeEditor::PinKind::Input);
		ImGui::Text("Scale In");
		ax::NodeEditor::EndPin();

		// Drop down
		if (ImGui::TreeNode("Resolved Transform"))
		{
			// Display the resolved transform
			ImGui::Text("Pos: %.2f, %.2f, %.2f", node.resolvedPos.m_x, node.resolvedPos.m_y, node.resolvedPos.m_z);
			ImGui::Text("Rot: %.2f, %.2f, %.2f", node.resolvedRot.m_x, node.resolvedRot.m_y, node.resolvedRot.m_z);
			ImGui::Text("Scale: %.2f, %.2f, %.2f", node.resolvedScale.m_x, node.resolvedScale.m_y, node.resolvedScale.m_z);

			ImGui::TreePop();
		}
		
		ImGui::PopID();
		ax::NodeEditor::EndNode();
		return;
	}

	// Handle math operation nodes (Add, Subtract, Multiply, Divide)
	// Draw title
	switch (node.type)
	{
	case MathOpType::Add: ImGui::Text("Add"); break;
	case MathOpType::Subtract: ImGui::Text("Subtract"); break;
	case MathOpType::Multiply: ImGui::Text("Multiply"); break;
	case MathOpType::Divide: ImGui::Text("Divide"); break;
	default: ImGui::Text("Unknown"); break;
	}

	ImGui::PushItemWidth(90.0f);

	int16_t resolvedValue;

	// ----- Input A -----
	ax::NodeEditor::BeginPin(node.inputA_id, ax::NodeEditor::PinKind::Input);
	if (ResolveInputValue(node.inputA_id, mathNodes, links, resolvedValue))
	{
		node.inputA = resolvedValue;
		ImGui::Text("A = %d", resolvedValue);  // Show the resolved value
	}
	else
	{
		ImGui::DragScalar("A", ImGuiDataType_S16, &node.inputA);  // Allow editing if no link
	}
	ax::NodeEditor::EndPin();

	// ----- Input B -----
	ax::NodeEditor::BeginPin(node.inputB_id, ax::NodeEditor::PinKind::Input);
	if (ResolveInputValue(node.inputB_id, mathNodes, links, resolvedValue))
	{
		node.inputB = resolvedValue;
		ImGui::Text("B = %d", resolvedValue);  // Show the resolved value
	}
	else
	{
		ImGui::DragScalar("B", ImGuiDataType_S16, &node.inputB);  // Allow editing if no link
	}
	ax::NodeEditor::EndPin();

	// ----- Output pin and result calculation -----
	// Output pin for inputA
	ax::NodeEditor::BeginPin(node.outputA_id, ax::NodeEditor::PinKind::Output);
	ImGui::Text("Out A: %d", node.inputA);
	ax::NodeEditor::EndPin();

	// Output pin for inputB
	ax::NodeEditor::BeginPin(node.outputB_id, ax::NodeEditor::PinKind::Output);
	ImGui::Text("Out B: %d", node.inputB);
	ax::NodeEditor::EndPin();

	// Output pin for result
	ax::NodeEditor::BeginPin(node.outputResult_id, ax::NodeEditor::PinKind::Output);
	ImGui::Text("Result: %d", node.result);
	ax::NodeEditor::EndPin();

	ImGui::PopID();
	ax::NodeEditor::EndNode();
}

// For Calculator
int16_t ResolveInputValue(int pinId, const std::vector<MathNode>& mathNodes, const std::vector<Link>& links, int16_t& outValue)
{
	for (const Link& link : links)
	{
		if (link.endPinId == pinId)
		{
			int sourcePinId = link.startPinId;
			for (const MathNode& node : mathNodes)
			{
				//std::cout << "Comparing node.output_id " << node.output_id << " with sourcePinId " << sourcePinId << std::endl;

				if (node.outputA_id == sourcePinId)
				{
					outValue = node.inputA;
					return true;
				}
				if (node.outputB_id == sourcePinId)
				{
					outValue = node.inputB;
					return true;
				}
				if (node.outputResult_id == sourcePinId)
				{
					outValue = node.result;
					return true;
				}
			}
		}
	}
	return false;
}

// For Cube Transforms
bool ResolveVectorInput(int pinId, const std::vector<MathNode>& mathNodes, const std::vector<Link>& links, Vector3D& outValue)
{
	for (const Link& link : links)
	{
		if (link.endPinId == pinId)
		{
			int sourcePinId = link.startPinId;
			for (const MathNode& node : mathNodes)
			{
				if (node.outputPos_id == sourcePinId)
				{
					outValue = node.position;
					return true;
				}
				if (node.outputRot_id == sourcePinId)
				{
					outValue = node.rotation;
					return true;
				}
				if (node.outputScale_id == sourcePinId)
				{
					outValue = node.scale;
					return true;
				}
			}
		}
	}
	return false;
}

AppWindow::~AppWindow()
{
}