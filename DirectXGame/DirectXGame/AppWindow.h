#pragma once

#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Matrix4x4.h"

#include <vector>
#include "Cube.h"

#include "Vector2D.h"
#include "ScreenQuad.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

class WireframeRenderer;
class AGameObject;
class ScreenQuad;

__declspec(align(16)) struct PostProcessData
{
	Vector2D resolution;
	float chromaAmount = 2.0f;
	float pad; //Padding to align to 16 bytes
};

class AppWindow: public Window, public InputListener
{
public:
	AppWindow();

	void update();

	~AppWindow();

	// Inherited via Window
	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;

	// Inherited via InputListener
	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point& mouse_pos) override;

	virtual void onLeftMouseDown(const Point& mouse_pos) override;
	virtual void onLeftMouseUp(const Point& mouse_pos) override;

	virtual void onRightMouseDown(const Point& mouse_pos) override;
	virtual void onRightMouseUp(const Point& mouse_pos) override;

	bool m_cursorEnabled = false;

	UINT size_list;
private:
	SwapChain* m_swap_chain;
	VertexBuffer* m_vb;
	VertexShader* m_vs;
	PixelShader* m_ps;
	ConstantBuffer* m_cb;
	IndexBuffer* m_ib;

	ConstantBuffer* m_postProcessCB = nullptr;
	//VertexBuffer* m_instanceBuffer = nullptr;
private:
	float m_old_delta;
	float m_new_delta;
	float m_delta_time;

	float m_delta_pos;
	float m_delta_scale;

	float m_rot_x = 0.0f;
	float m_rot_y = 0.0f;

	float m_scale_cube = 1;
	float m_forward = 0.0f;
	float m_rightward = 0.0f;

	Matrix4x4 m_world_cam;
private:
	WireframeRenderer* m_wireframe_renderer = nullptr;
//Post Process
private:
	float m_chromaAmount = 2.0f;

	ScreenQuad* m_screenQuad = nullptr;
	VertexShader* m_postProcessVS;
	PixelShader* m_postProcessPS;

	ID3D11Texture2D* m_offscreenTexture = nullptr;
	ID3D11RenderTargetView* m_offscreenRTV = nullptr;
	ID3D11ShaderResourceView* m_offscreenSRV = nullptr;
private:
	std::vector<Cube*> cubeList;
	std::vector<AGameObject*> cubeList2;
};

