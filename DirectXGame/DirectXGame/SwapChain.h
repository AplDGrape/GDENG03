#pragma once
#include <d3d11.h>

class DeviceContext;

class SwapChain
{
public:
	SwapChain();
	//Initialize SwapChain for a Window
	bool init(HWND hwnd, UINT width, UINT height);

	bool present(bool vsync);

	ID3D11RenderTargetView* getRenderTargetView();
	ID3D11DepthStencilView* getDepthStencilView();

	//Release the swap chain
	bool release();
	~SwapChain();
private:
	IDXGISwapChain* m_swap_chain;
	ID3D11RenderTargetView* m_rtv;

	ID3D11Texture2D* m_depth_buffer = nullptr;
	ID3D11DepthStencilView* m_dsv;
private:
	friend class DeviceContext;
};
