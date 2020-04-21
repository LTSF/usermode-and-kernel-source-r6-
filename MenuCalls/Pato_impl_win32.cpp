// dear Pato: Platform Binding for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core Pato)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= PatoConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. Pato::IsKeyPressed(VK_SPACE).
// Missing features:
//  [ ] Platform: Gamepad support (best leaving it to user application to fill io.NavInputs[] with gamepad inputs from their source of choice).

#include "Pato.h"
#include "Pato_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-06-29: Inputs: Added support for the PatoMouseCursor_Hand cursor.
//  2018-06-10: Inputs: Fixed handling of mouse wheel messages to support fine position messages (typically sent by track-pads).
//  2018-06-08: Misc: Extracted Pato_impl_win32.cpp/.h away from the old combined DX9/DX10/DX11/DX12 examples.
//  2018-03-20: Misc: Setup io.BackendFlags PatoBackendFlags_HasMouseCursors and PatoBackendFlags_HasSetMousePos flags + honor PatoConfigFlags_NoMouseCursorChange flag.
//  2018-02-20: Inputs: Added support for mouse cursors (Pato::GetMouseCursor() value and WM_SETCURSOR message handling).
//  2018-02-06: Inputs: Added mapping for PatoKey_Space.
//  2018-02-06: Inputs: Honoring the io.WantSetMousePos by repositioning the mouse (when using navigation and PatoConfigFlags_NavMoveMouse is set).
//  2018-02-06: Misc: Removed call to Pato::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-08: Inputs: Added mapping for PatoKey_Insert.
//  2018-01-05: Inputs: Added WM_LBUTTONDBLCLK double-click handlers for window classes with the CS_DBLCLKS flag.
//  2017-10-23: Inputs: Added WM_SYSKEYDOWN / WM_SYSKEYUP handlers so e.g. the VK_MENU key can be read.
//  2017-10-23: Inputs: Using Win32 ::SetCapture/::GetCapture() to retrieve mouse positions outside the client area when dragging. 
//  2016-11-12: Inputs: Only call Win32 ::SetCursor(NULL) when io.MouseDrawCursor is set.

// Win32 Data
static HWND                 g_hWnd = 0;
static INT64                g_Time = 0;
static INT64                g_TicksPerSecond = 0;
static PatoMouseCursor     g_LastMouseCursor = PatoMouseCursor_COUNT;

// Functions
bool    Pato_ImplWin32_Init(void* hwnd)
{
    if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
        return false;

    // Setup back-end capabilities flags
    g_hWnd = (HWND)hwnd;
    PatoIO& io = Pato::GetIO();
    io.BackendFlags |= PatoBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= PatoBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "Pato_impl_win32";
    io.ImeWindowHandle = hwnd;

    // Keyboard mapping. Pato will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[PatoKey_Tab] = VK_TAB;
    io.KeyMap[PatoKey_LeftArrow] = VK_LEFT;
    io.KeyMap[PatoKey_RightArrow] = VK_RIGHT;
    io.KeyMap[PatoKey_UpArrow] = VK_UP;
    io.KeyMap[PatoKey_DownArrow] = VK_DOWN;
    io.KeyMap[PatoKey_PageUp] = VK_PRIOR;
    io.KeyMap[PatoKey_PageDown] = VK_NEXT;
    io.KeyMap[PatoKey_Home] = VK_HOME;
    io.KeyMap[PatoKey_End] = VK_END;
    io.KeyMap[PatoKey_Insert] = VK_INSERT;
    io.KeyMap[PatoKey_Delete] = VK_DELETE;
    io.KeyMap[PatoKey_Backspace] = VK_BACK;
    io.KeyMap[PatoKey_Space] = VK_SPACE;
    io.KeyMap[PatoKey_Enter] = VK_RETURN;
    io.KeyMap[PatoKey_Escape] = VK_ESCAPE;
    io.KeyMap[PatoKey_A] = 'A';
    io.KeyMap[PatoKey_C] = 'C';
    io.KeyMap[PatoKey_V] = 'V';
    io.KeyMap[PatoKey_X] = 'X';
    io.KeyMap[PatoKey_Y] = 'Y';
    io.KeyMap[PatoKey_Z] = 'Z';

    return true;
}

void    Pato_ImplWin32_Shutdown()
{
    g_hWnd = (HWND)0;
}

static bool Pato_ImplWin32_UpdateMouseCursor()
{
    PatoIO& io = Pato::GetIO();
    if (io.ConfigFlags & PatoConfigFlags_NoMouseCursorChange)
        return false;

    PatoMouseCursor Pato_cursor = Pato::GetMouseCursor();
    if (Pato_cursor == PatoMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if Pato is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (Pato_cursor)
        {
        case PatoMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
        case PatoMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
        case PatoMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
        case PatoMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
        case PatoMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
        case PatoMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
        case PatoMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
        case PatoMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
    return true;
}

static void Pato_ImplWin32_UpdateMousePos()
{
    PatoIO& io = Pato::GetIO();

    // Set OS mouse position if requested (rarely used, only when PatoConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
    {
        POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
        ::ClientToScreen(g_hWnd, &pos);
        ::SetCursorPos(pos.x, pos.y);
    }

    // Set mouse position
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    POINT pos;
    if (::GetActiveWindow() == g_hWnd && ::GetCursorPos(&pos))
        if (::ScreenToClient(g_hWnd, &pos))
            io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}

void    Pato_ImplWin32_NewFrame()
{
    PatoIO& io = Pato::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    ::GetClientRect(g_hWnd, &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    // Setup time step
    INT64 current_time;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
    g_Time = current_time;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;
    // io.KeysDown[], io.MousePos, io.MouseDown[], io.MouseWheel: filled by the WndProc handler below.

    // Update OS mouse position
    Pato_ImplWin32_UpdateMousePos();

    // Update OS mouse cursor with the cursor requested by Pato
    PatoMouseCursor mouse_cursor = io.MouseDrawCursor ? PatoMouseCursor_None : Pato::GetMouseCursor();
    if (g_LastMouseCursor != mouse_cursor)
    {
        g_LastMouseCursor = mouse_cursor;
        Pato_ImplWin32_UpdateMouseCursor();
    }
}

// Allow compilation with old Windows SDK. MinGW doesn't have default _WIN32_WINNT/WINVER versions.
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif

// Process Win32 mouse/keyboard inputs. 
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear Pato wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear Pato, and hide them from your application based on those two flags.
// PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinations when dragging mouse outside of our window bounds.
// PS: We treat DBLCLK messages as regular mouse down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.
Pato_IMPL_API LRESULT Pato_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Pato::GetCurrentContext() == NULL)
        return 0;

    PatoIO& io = Pato::GetIO();
    switch (msg)
    {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    {
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
        if (!Pato::IsAnyMouseDown() && ::GetCapture() == NULL)
            ::SetCapture(hwnd);
        io.MouseDown[button] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
        int button = 0;
        if (msg == WM_LBUTTONUP) button = 0;
        if (msg == WM_RBUTTONUP) button = 1;
        if (msg == WM_MBUTTONUP) button = 2;
        io.MouseDown[button] = false;
        if (!Pato::IsAnyMouseDown() && ::GetCapture() == hwnd)
            ::ReleaseCapture();
        return 0;
    }
    case WM_MOUSEWHEEL:
        io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_MOUSEHWHEEL:
        io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return 0;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT && Pato_ImplWin32_UpdateMouseCursor())
            return 1;
        return 0;
    }
    return 0;
}

