#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <io.h>
#include <cstdint>
#include <cstdlib>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <tlhelp32.h>
#include <random>
#include <string_view>

#include "xorstr.hpp"

#include <dwmapi.h>
#include <comdef.h> 
#include <d3d9.h>
#include <C:\Users\imjus\Downloads\unfair.cc\noway\Include\d3dx9.h>
#pragma comment(lib, "d3d9.lib")


#include "MenuCalls/Pato.h"
#include "MenuCalls/Pato_impl_dx9.h"
#include "MenuCalls/Pato_impl_win32.h"

#include "render.h"
#include "kernel.h"
#include "globals.h"
#include "pattern.h"
#include "math.h"
#include "engine.h"
#include "menu.h"
#include "draw.h"
#pragma comment(lib, "dwmapi.lib")

const MARGINS Margin = { -1 };

HRESULT DirectXInit(HWND hWnd);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SetupWindow();
WPARAM MainLoop();
void CleanuoD3D();

int isTopwin();
void SetWindowToTarget();
void create_console();


#define TopWindowGame 11
#define TopWindowMvoe 22

DWORD ScreenCenterX;
DWORD ScreenCenterY;

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };
std::string comcalma;
std::string opatudobem;
#define M_Name (opatudobem.c_str())
HWND MyWnd = NULL;
HWND GameWnd = NULL;
HWND GameWindow = NULL;
RECT GameRect = { NULL };
MSG Message = { NULL };

extern LRESULT Pato_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);