#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include "Game_Instance.h"
#include "Client_Define.h"
#include "Client_Function.h"
#include "Client_Macro.h"
#include "Info.h"
#include "Inven.h"

// console
//#include <iostream>
//#ifdef UNICODE
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#else
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
#endif

// dialog
#include <commdlg.h>
// kor
#include <codecvt>

#pragma comment(lib, "shlwapi.lib")
#include <Shlwapi.h>
