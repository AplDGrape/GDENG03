#pragma once
#pragma comment(lib,"d3d11.lib")

#include <Windows.h>

class Window
{
public:
	Window();

	//Initialize the window
	bool init();
	bool broadcast();

	//Release the window
	bool release();
	bool isRun();

	RECT getClientWindowRect();
	void setHWND(HWND hwnd);

	//EVENTS
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();

	~Window();

protected:
	HWND m_hwnd;
	bool m_is_run;
};

