#pragma once

#include <process.h>

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern bool g_bChangeScene;

namespace Client {
	//const unsigned int g_iWinSizeX = 1600;
	//const unsigned int g_iWinSizeY = 900;

	//const unsigned int g_iWinSizeX = 1920;
	//const unsigned int g_iWinSizeY = 1080;
	 
	const unsigned int g_iWinSizeX = 2560;
	const unsigned int g_iWinSizeY = 1440;
	 
	//const unsigned int g_iWinSizeX = 3840;
	//const unsigned int g_iWinSizeY = 2160;
	
	enum class EScene : unsigned char { STATIC, LOADING, LOGO, LOBBY, GAMEPLAY, END };
}

using namespace Client;