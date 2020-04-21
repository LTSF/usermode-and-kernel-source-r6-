// dear Pato, v1.67 WIP
// (headers)

// See Pato.cpp file for documentation.
// Call and read Pato::ShowDemoWindow() in Pato_demo.cpp for demo code.
// Read 'Programmer guide' in Pato.cpp for notes on how to setup Pato in your codebase.
// Get latest version at https://github.com/ocornut/Pato

/*

Index of this file:
// Header mess
// Forward declarations and basic types
// Pato API (Dear Pato end-user API)
// Flags & Enumerations
// PatoStyle
// PatoIO
// Misc data structures (PatoInputTextCallbackData, PatoSizeCallbackData, PatoPayload)
// Obsolete functions
// Helpers (ImVector, PatoOnceUponAFrame, PatoTextFilter, PatoTextBuffer, PatoStorage, PatoListClipper, ImColor)
// Draw List API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListFlags, ImDrawList, ImDrawData)
// Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFont)

*/

#pragma once
#include <Windows.h>
// Configuration file (edit imconfig.h or define Pato_USER_CONFIG to your own filename)
#ifdef Pato_USER_CONFIG
#include Pato_USER_CONFIG
#endif
#if !defined(Pato_DISABLE_INCLUDE_IMCONFIG_H) || defined(Pato_INCLUDE_IMCONFIG_H)
#include "imconfig.h"
#endif

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

#include <float.h>                  // FLT_MAX
#include <stdarg.h>                 // va_list
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals. Work in progress versions typically starts at XYY00 then bounced up to XYY01 when release tagging happens)
#define Pato_VERSION               "1.67 WIP"
#define Pato_VERSION_NUM           16602
#define Pato_CHECKVERSION()        Pato::DebugCheckVersionAndDataLayout(Pato_VERSION, sizeof(PatoIO), sizeof(PatoStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert))

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// Pato_API is used for core Pato functions, Pato_IMPL_API is used for the default bindings files (Pato_impl_xxx.h)
#ifndef Pato_API
#define Pato_API
#endif
#ifndef Pato_IMPL_API
#define Pato_IMPL_API              Pato_API
#endif

// Helper Macros
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#if defined(__clang__) || defined(__GNUC__)
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1))) // Apply printf-style warnings to user functions.
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))         // Size of a static C-style array. Don't use on pointers!
#define IM_OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Standardized as offsetof() in modern C++.

// Warnings
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

//-----------------------------------------------------------------------------
// Forward declarations and basic types
//-----------------------------------------------------------------------------

struct ImDrawChannel;               // Temporary storage for outputting drawing commands out of order, used by ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call)
struct ImDrawData;                  // All draw command lists required to render the frame
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent Pato context, but you may create one yourself)
struct ImDrawVert;                  // A single vertex (20 bytes by default, override layout with Pato_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*obsolete* please avoid using)
#ifndef ImTextureID
typedef void* ImTextureID;          // User data to identify a texture (this is whatever to you want it to be! read the FAQ about ImTextureID in Pato.cpp)
#endif
struct PatoContext;                // Pato context (opaque)
struct PatoIO;                     // Main configuration and I/O between your application and Pato
struct PatoInputTextCallbackData;  // Shared state of InputText() when using custom PatoInputTextCallback (rare/advanced use)
struct PatoListClipper;            // Helper to manually clip large list of items
struct PatoOnceUponAFrame;         // Helper for running a block of code not more than once a frame, used by Pato_ONCE_UPON_A_FRAME macro
struct PatoPayload;                // User data payload for drag and drop operations
struct PatoSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct PatoStorage;                // Helper for key->value storage
struct PatoStyle;                  // Runtime data for styling/colors
struct PatoTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbb][,ccccc]")
struct PatoTextBuffer;             // Helper to hold and append into a text buffer (~string builder)

// Typedefs and Enums/Flags (declared as int for compatibility with old C++, to allow using as flags and to not pollute the top of this file)
// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
typedef unsigned int PatoID;       // Unique ID used by widgets (typically hashed from a stack of string)
typedef unsigned short ImWchar;     // Character for keyboard input/display
typedef int PatoCol;               // -> enum PatoCol_             // Enum: A color identifier for styling
typedef int PatoCond;              // -> enum PatoCond_            // Enum: A condition for Set*()
typedef int PatoDataType;          // -> enum PatoDataType_        // Enum: A primary data type
typedef int PatoDir;               // -> enum PatoDir_             // Enum: A cardinal direction
typedef int PatoKey;               // -> enum PatoKey_             // Enum: A key identifier (Pato-side enum)
typedef int PatoNavInput;          // -> enum PatoNavInput_        // Enum: An input identifier for navigation
typedef int PatoMouseCursor;       // -> enum PatoMouseCursor_     // Enum: A mouse cursor identifier
typedef int PatoStyleVar;          // -> enum PatoStyleVar_        // Enum: A variable identifier for styling
typedef int ImDrawCornerFlags;      // -> enum ImDrawCornerFlags_    // Flags: for ImDrawList::AddRect*() etc.
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas
typedef int PatoBackendFlags;      // -> enum PatoBackendFlags_    // Flags: for io.BackendFlags
typedef int PatoColorEditFlags;    // -> enum PatoColorEditFlags_  // Flags: for ColorEdit*(), ColorPicker*()
typedef int PatoColumnsFlags;      // -> enum PatoColumnsFlags_    // Flags: for Columns(), BeginColumns()
typedef int PatoConfigFlags;       // -> enum PatoConfigFlags_     // Flags: for io.ConfigFlags
typedef int PatoComboFlags;        // -> enum PatoComboFlags_      // Flags: for BeginCombo()
typedef int PatoDragDropFlags;     // -> enum PatoDragDropFlags_   // Flags: for *DragDrop*()
typedef int PatoFocusedFlags;      // -> enum PatoFocusedFlags_    // Flags: for IsWindowFocused()
typedef int PatoHoveredFlags;      // -> enum PatoHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int PatoInputTextFlags;    // -> enum PatoInputTextFlags_  // Flags: for InputText*()
typedef int PatoSelectableFlags;   // -> enum PatoSelectableFlags_ // Flags: for Selectable()
typedef int PatoTabBarFlags;       // -> enum PatoTabBarFlags_     // Flags: for BeginTabBar()
typedef int PatoTabItemFlags;      // -> enum PatoTabItemFlags_    // Flags: for BeginTabItem()
typedef int PatoTreeNodeFlags;     // -> enum PatoTreeNodeFlags_   // Flags: for TreeNode*(),CollapsingHeader()
typedef int PatoWindowFlags;       // -> enum PatoWindowFlags_     // Flags: for Begin*()
typedef int (*PatoInputTextCallback)(PatoInputTextCallbackData *data);
typedef void (*PatoSizeCallback)(PatoSizeCallbackData* data);

// Scalar data types
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
#if defined(_MSC_VER) && !defined(__clang__)
typedef signed   __int64    ImS64;  // 64-bit signed integer (pre and post C++11 with Visual Studio)
typedef unsigned __int64    ImU64;  // 64-bit unsigned integer (pre and post C++11 with Visual Studio)
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
#include <stdint.h>
typedef int64_t             ImS64;  // 64-bit signed integer (pre C++11)
typedef uint64_t            ImU64;  // 64-bit unsigned integer (pre C++11)
#else
typedef signed   long long  ImS64;  // 64-bit signed integer (post C++11)
typedef unsigned long long  ImU64;  // 64-bit unsigned integer (post C++11)
#endif

// 2D vector (often used to store positions, sizes, etc.)
struct ImVec2
{
    float     x, y;
    ImVec2()  { x = y = 0.0f; }
    ImVec2(float _x, float _y) { x = _x; y = _y; }
    float operator[] (size_t i) const { IM_ASSERT(i <= 1); return (&x)[i]; }    // We very rarely use this [] operator, the assert overhead is fine.
#ifdef IM_VEC2_CLASS_EXTRA
    IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// 4D vector (often used to store floating-point colors)
struct ImVec4
{
    float     x, y, z, w;
    ImVec4()  { x = y = z = w = 0.0f; }
    ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
#ifdef IM_VEC4_CLASS_EXTRA
    IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};

//-----------------------------------------------------------------------------
// Pato: Dear Pato end-user API
// (Inside a namespace so you can add extra functions in your own separate file. Please don't modify Pato.cpp/.h!)
//-----------------------------------------------------------------------------

namespace Pato
{
    // Context creation and access
    // Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between Pato contexts.
    // All those functions are not reliant on the current context.
    Pato_API PatoContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
    Pato_API void          DestroyContext(PatoContext* ctx = NULL);   // NULL = destroy current context
    Pato_API PatoContext* GetCurrentContext();
    Pato_API void          SetCurrentContext(PatoContext* ctx);
    Pato_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert);

    // Main
    Pato_API PatoIO&      GetIO();                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
    Pato_API PatoStyle&   GetStyle();                                 // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame.
    Pato_API void          NewFrame();                                 // start a new Pato frame, you can submit any command from this point until Render()/EndFrame().
    Pato_API void          EndFrame();                                 // ends the Pato frame. automatically called by Render(), you likely don't need to call that yourself directly. If you don't need to render data (skipping rendering) you may call EndFrame() but you'll have wasted CPU already! If you don't need to render, better to not create any Pato windows and not call NewFrame() at all!
    Pato_API void          Render();                                   // ends the Pato frame, finalize the draw data. (Obsolete: optionally call io.RenderDrawListsFn if set. Nowadays, prefer calling your render function yourself.)
    Pato_API ImDrawData*   GetDrawData();                              // valid after Render() and until the next call to NewFrame(). this is what you have to render. (Obsolete: this used to be passed to your io.RenderDrawListsFn() function.)

    // Demo, Debug, Information
    Pato_API void          ShowDemoWindow(bool* p_open = NULL);        // create demo/test window (previously called ShowTestWindow). demonstrate most Pato features. call this to learn about the library! try to make it always available in your application!
    Pato_API void          ShowAboutWindow(bool* p_open = NULL);       // create about window. display Dear Pato version, credits and build/system information.
    Pato_API void          ShowMetricsWindow(bool* p_open = NULL);     // create metrics window. display Dear Pato internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
    Pato_API void          ShowStyleEditor(PatoStyle* ref = NULL);    // add style editor block (not a window). you can pass in a reference PatoStyle structure to compare to, revert to and save to (else it uses the default style)
    Pato_API bool          ShowStyleSelector(const char* label);       // add style selector block (not a window), essentially a combo listing the default styles.
    Pato_API void          ShowFontSelector(const char* label);        // add font selector block (not a window), essentially a combo listing the loaded fonts.
    Pato_API void          ShowUserGuide();                            // add basic help/info block (not a window): how to manipulate Pato as a end-user (mouse/keyboard controls).
    Pato_API const char*   GetVersion();                               // get the compiled version string e.g. "1.23"

    // Styles
    Pato_API void          StyleColorsDark(PatoStyle* dst = NULL);    // new, recommended style (default)
    Pato_API void          StyleColorsClassic(PatoStyle* dst = NULL); // classic Pato style
    Pato_API void          StyleColorsLight(PatoStyle* dst = NULL);   // best used with borders and a custom, thicker font

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - You may append multiple times to the same window during the same frame.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window, 
    //   which clicking will set the boolean to false when clicked.
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. 
    //    where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
    // - Note that the bottom of window stack always contains a window called "Debug".
    Pato_API bool          Begin(const char* name, bool* p_open = NULL, PatoWindowFlags flags = 0);
    Pato_API void          End();

    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
    //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
    Pato_API bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0,0), bool border = false, PatoWindowFlags flags = 0);
    Pato_API bool          BeginChild(PatoID id, const ImVec2& size = ImVec2(0,0), bool border = false, PatoWindowFlags flags = 0);
    Pato_API void          EndChild();

    // Windows Utilities
    // "current window" = the window we are appending into while inside a Begin()/End() block. "next window" = next window we will Begin() into.
    Pato_API bool          IsWindowAppearing();
    Pato_API bool          IsWindowCollapsed();
    Pato_API bool          IsWindowFocused(PatoFocusedFlags flags=0); // is current window focused? or its root/child, depending on flags. see flags for options.
    Pato_API bool          IsWindowHovered(PatoHoveredFlags flags=0); // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to Pato or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
    Pato_API ImDrawList*   GetWindowDrawList();                        // get draw list associated to the window, to append your own drawing primitives
    Pato_API ImVec2        GetWindowPos();                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
    Pato_API ImVec2        GetWindowSize();                            // get current window size
    Pato_API float         GetWindowWidth();                           // get current window width (shortcut for GetWindowSize().x)
    Pato_API float         GetWindowHeight();                          // get current window height (shortcut for GetWindowSize().y)
    Pato_API ImVec2        GetContentRegionMax();                      // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
    Pato_API ImVec2        GetContentRegionAvail();                    // == GetContentRegionMax() - GetCursorPos()
    Pato_API float         GetContentRegionAvailWidth();               //
    Pato_API ImVec2        GetWindowContentRegionMin();                // content boundaries min (roughly (0,0)-Scroll), in window coordinates
    Pato_API ImVec2        GetWindowContentRegionMax();                // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
    Pato_API float         GetWindowContentRegionWidth();              //

    Pato_API void          SetNextWindowPos(const ImVec2& pos, PatoCond cond = 0, const ImVec2& pivot = ImVec2(0,0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    Pato_API void          SetNextWindowSize(const ImVec2& size, PatoCond cond = 0);                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
    Pato_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, PatoSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Use callback to apply non-trivial programmatic constraints.
    Pato_API void          SetNextWindowContentSize(const ImVec2& size);                               // set next window content size (~ enforce the range of scrollbars). not including window decorations (title bar, menu bar, etc.). set an axis to 0.0f to leave it automatic. call before Begin()
    Pato_API void          SetNextWindowCollapsed(bool collapsed, PatoCond cond = 0);                 // set next window collapsed state. call before Begin()
    Pato_API void          SetNextWindowFocus();                                                       // set next window to be focused / front-most. call before Begin()
    Pato_API void          SetNextWindowBgAlpha(float alpha);                                          // set next window background color alpha. helper to easily modify PatoCol_WindowBg/ChildBg/PopupBg. you may also use PatoWindowFlags_NoBackground.
    Pato_API void          SetWindowPos(const ImVec2& pos, PatoCond cond = 0);                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
    Pato_API void          SetWindowSize(const ImVec2& size, PatoCond cond = 0);                      // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
    Pato_API void          SetWindowCollapsed(bool collapsed, PatoCond cond = 0);                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
    Pato_API void          SetWindowFocus();                                                           // (not recommended) set current window to be focused / front-most. prefer using SetNextWindowFocus().
    Pato_API void          SetWindowFontScale(float scale);                                            // set font scale. Adjust IO.FontGlobalScale if you want to scale all windows
    Pato_API void          SetWindowPos(const char* name, const ImVec2& pos, PatoCond cond = 0);      // set named window position.
    Pato_API void          SetWindowSize(const char* name, const ImVec2& size, PatoCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
    Pato_API void          SetWindowCollapsed(const char* name, bool collapsed, PatoCond cond = 0);   // set named window collapsed state
    Pato_API void          SetWindowFocus(const char* name);                                           // set named window to be focused / front-most. use NULL to remove focus.

    // Windows Scrolling
    Pato_API float         GetScrollX();                                                   // get scrolling amount [0..GetScrollMaxX()]
    Pato_API float         GetScrollY();                                                   // get scrolling amount [0..GetScrollMaxY()]
    Pato_API float         GetScrollMaxX();                                                // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
    Pato_API float         GetScrollMaxY();                                                // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
    Pato_API void          SetScrollX(float scroll_x);                                     // set scrolling amount [0..GetScrollMaxX()]
    Pato_API void          SetScrollY(float scroll_y);                                     // set scrolling amount [0..GetScrollMaxY()]
    Pato_API void          SetScrollHereY(float center_y_ratio = 0.5f);                     // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    Pato_API void          SetScrollFromPosY(float pos_y, float center_y_ratio = 0.5f);    // adjust scrolling amount to make given position valid. use GetCursorPos() or GetCursorLoopCallbackPos()+offset to get valid positions.

    // Parameters stacks (shared)
    Pato_API void          PushFont(ImFont* font);                                         // use NULL as a shortcut to push default font
    Pato_API void          PopFont();
    Pato_API void          PushStyleColor(PatoCol idx, ImU32 col);
    Pato_API void          PushStyleColor(PatoCol idx, const ImVec4& col);
    Pato_API void          PopStyleColor(int count = 1);
    Pato_API void          PushStyleVar(PatoStyleVar idx, float val);
    Pato_API void          PushStyleVar(PatoStyleVar idx, const ImVec2& val);
    Pato_API void          PopStyleVar(int count = 1);
    Pato_API const ImVec4& GetStyleColorVec4(PatoCol idx);                                // retrieve style color as stored in PatoStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in.
    Pato_API ImFont*       GetFont();                                                      // get current font
    Pato_API float         GetFontSize();                                                  // get current font size (= height in pixels) of current font with current scale applied
    Pato_API ImVec2        GetFontTexUvWhitePixel();                                       // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
    Pato_API ImU32         GetColorU32(PatoCol idx, float alpha_mul = 1.0f);              // retrieve given style color with style alpha applied and optional extra alpha multiplier
    Pato_API ImU32         GetColorU32(const ImVec4& col);                                 // retrieve given color with style alpha applied
    Pato_API ImU32         GetColorU32(ImU32 col);                                         // retrieve given color with style alpha applied

    // Parameters stacks (current window)
    Pato_API void          PushItemWidth(float item_width);                                // width of items for the common item+label case, pixels. 0.0f = default to ~2/3 of windows width, >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
    Pato_API void          PopItemWidth();
    Pato_API float         CalcItemWidth();                                                // width of item given pushed settings and current cursor position
    Pato_API void          PushTextWrapPos(float wrap_pos_x = 0.0f);                       // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
    Pato_API void          PopTextWrapPos();
    Pato_API void          PushAllowKeyboardFocus(bool allow_keyboard_focus);              // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
    Pato_API void          PopAllowKeyboardFocus();
    Pato_API void          PushButtonRepeat(bool repeat);                                  // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
    Pato_API void          PopButtonRepeat();

    // Cursor / Layout
    Pato_API void          Separator();                                                    // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
    Pato_API void          SameLine(float pos_x = 0.0f, float spacing_w = -1.0f);          // call between widgets or groups to layout them horizontally
    Pato_API void          NewLine();                                                      // undo a SameLine()
    Pato_API void          Spacing();                                                      // add vertical spacing
	Pato_API void          NewSpacing(float pos);                                                   // add vertical spacing
    Pato_API void          Dummy(const ImVec2& size);                                      // add a dummy item of given size
    Pato_API void          Indent(float indent_w = 0.0f);                                  // move content position toward the right, by style.IndentSpacing or indent_w if != 0
    Pato_API void          Unindent(float indent_w = 0.0f);                                // move content position back to the left, by style.IndentSpacing or indent_w if != 0
    Pato_API void          BeginGroup();                                                   // lock horizontal starting position + capture group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
    Pato_API void          EndGroup();
    Pato_API ImVec2        GetCursorPos();                                                 // cursor position is relative to window position
    Pato_API float         GetCursorPosX();                                                // "
    Pato_API float         GetCursorPosY();                                                // "
    Pato_API void          SetCursorPos(const ImVec2& local_pos);                          // "
    Pato_API void          SetCursorPosX(float x);                                         // "
    Pato_API void          SetCursorPosY(float y);                                         // "
    Pato_API ImVec2        GetCursorLoopCallbackPos();                                            // initial cursor position
    Pato_API ImVec2        GetCursorScreenPos();                                           // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)
    Pato_API void          SetCursorScreenPos(const ImVec2& screen_pos);                   // cursor position in absolute screen coordinates [0..io.DisplaySize]
    Pato_API void          AlignTextToFramePadding();                                      // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
    Pato_API float         GetTextLineHeight();                                            // ~ FontSize
    Pato_API float         GetTextLineHeightWithSpacing();                                 // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
    Pato_API float         GetFrameHeight();                                               // ~ FontSize + style.FramePadding.y * 2
    Pato_API float         GetFrameHeightWithSpacing();                                    // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

    // ID stack/scopes
    // Read the FAQ for more details about how ID are handled in dear Pato. If you are creating widgets in a loop you most
    // likely want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
    // You can also use the "##foobar" syntax within widget label to distinguish them from each others.
    // In this header file we use the "label"/"name" terminology to denote a string that will be displayed and used as an ID,
    // whereas "str_id" denote a string that is only used as an ID and not aimed to be displayed.
    Pato_API void          PushID(const char* str_id);                                     // push identifier into the ID stack. IDs are hash of the entire stack!
    Pato_API void          PushID(const char* str_id_begin, const char* str_id_end);
    Pato_API void          PushID(const void* ptr_id);
    Pato_API void          PushID(int int_id);
    Pato_API void          PopID();
    Pato_API PatoID       GetID(const char* str_id);                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into PatoStorage yourself
    Pato_API PatoID       GetID(const char* str_id_begin, const char* str_id_end);
    Pato_API PatoID       GetID(const void* ptr_id);

    // Widgets: Text
    Pato_API void          TextUnformatted(const char* text, const char* text_end = NULL);                // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
    Pato_API void          Text(const char* fmt, ...)                                      IM_FMTARGS(1); // simple formatted text
    Pato_API void          TextV(const char* fmt, va_list args)                            IM_FMTLIST(1);
    Pato_API void          TextColored(const ImVec4& col, const char* fmt, ...)            IM_FMTARGS(2); // shortcut for PushStyleColor(PatoCol_Text, col); Text(fmt, ...); PopStyleColor();
    Pato_API void          TextColoredV(const ImVec4& col, const char* fmt, va_list args)  IM_FMTLIST(2);
    Pato_API void          TextDisabled(const char* fmt, ...)                              IM_FMTARGS(1); // shortcut for PushStyleColor(PatoCol_Text, style.Colors[PatoCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
    Pato_API void          TextDisabledV(const char* fmt, va_list args)                    IM_FMTLIST(1);
    Pato_API void          TextWrapped(const char* fmt, ...)                               IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
    Pato_API void          TextWrappedV(const char* fmt, va_list args)                     IM_FMTLIST(1);
    Pato_API void          LabelText(const char* label, const char* fmt, ...)              IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets
    Pato_API void          LabelTextV(const char* label, const char* fmt, va_list args)    IM_FMTLIST(2);
    Pato_API void          BulletText(const char* fmt, ...)                                IM_FMTARGS(1); // shortcut for Bullet()+Text()
    Pato_API void          BulletTextV(const char* fmt, va_list args)                      IM_FMTLIST(1);

    // Widgets: Main
    // Most widgets return true when the value has been changed or when pressed/selected
    Pato_API bool          Button(const char* label, const ImVec2& size = ImVec2(0,0));    // button
    Pato_API bool          SmallButton(const char* label);                                 // button with FramePadding=(0,0) to easily embed within text
    Pato_API bool          InvisibleButton(const char* str_id, const ImVec2& size);        // button behavior without the visuals, useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
    Pato_API bool          ArrowButton(const char* str_id, PatoDir dir);                  // square button with an arrow shape
    Pato_API void          Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    Pato_API bool          ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
  
    Pato_API bool          Checkbox(const char* label, bool* v);
    Pato_API bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
    Pato_API bool          RadioButton(const char* label, bool active);                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
    Pato_API bool          RadioButton(const char* label, int* v, int v_button);           // shortcut to handle the above pattern when value is an integer
    Pato_API void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1,0), const char* overlay = NULL);
    Pato_API void          Bullet();                                                       // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

    // Widgets: Combo Box
    // The new BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
    // The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
    Pato_API bool          BeginCombo(const char* label, const char* preview_value, PatoComboFlags flags = 0);
    Pato_API void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
    Pato_API bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    Pato_API bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
    Pato_API bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

    // Widgets: Drags (tip: ctrl+click on a drag box to input with keyboard. manually input values aren't clamped, can go off-bounds)
    // For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
    // Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    // Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
    Pato_API bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
    Pato_API bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, float power = 1.0f);
    Pato_API bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");                                       // If v_min >= v_max we have no bound
    Pato_API bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    Pato_API bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    Pato_API bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    Pato_API bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL);
    Pato_API bool          DragScalar(const char* label, PatoDataType data_type, void* v, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);
    Pato_API bool          DragScalarN(const char* label, PatoDataType data_type, void* v, int components, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);

    // Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
    // Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    Pato_API bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
    Pato_API bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg");
    Pato_API bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d");
    Pato_API bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d");
    Pato_API bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d");
    Pato_API bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d");
    Pato_API bool          SliderScalar(const char* label, PatoDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
    Pato_API bool          SliderScalarN(const char* label, PatoDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
    Pato_API bool          VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    Pato_API bool          VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d");
    Pato_API bool          VSliderScalar(const char* label, const ImVec2& size, PatoDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);

    // Widgets: Input with Keyboard
    // If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/Pato_stdlib.h
    Pato_API bool          InputText(const char* label, char* buf, size_t buf_size, PatoInputTextFlags flags = 0, PatoInputTextCallback callback = NULL, void* user_data = NULL);
    Pato_API bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0,0), PatoInputTextFlags flags = 0, PatoInputTextCallback callback = NULL, void* user_data = NULL);
    Pato_API bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputFloat2(const char* label, float v[2], const char* format = "%.3f", PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputFloat3(const char* label, float v[3], const char* format = "%.3f", PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputFloat4(const char* label, float v[4], const char* format = "%.3f", PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputInt2(const char* label, int v[2], PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputInt3(const char* label, int v[3], PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputInt4(const char* label, int v[4], PatoInputTextFlags extra_flags = 0);
	Pato_API bool          Hotkey(const char* label, int* k, const ImVec2& size_arg);
    Pato_API bool          InputDouble(const char* label, double* v, double step = 0.0f, double step_fast = 0.0f, const char* format = "%.6f", PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputScalar(const char* label, PatoDataType data_type, void* v, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, PatoInputTextFlags extra_flags = 0);
    Pato_API bool          InputScalarN(const char* label, PatoDataType data_type, void* v, int components, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, PatoInputTextFlags extra_flags = 0);

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    // Note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can the pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
    Pato_API bool          ColorEdit3(const char* label, float col[3], PatoColorEditFlags flags = 0);
    Pato_API bool          ColorEdit4(const char* label, float col[4], PatoColorEditFlags flags = 0);
    Pato_API bool          ColorPicker3(const char* label, float col[3], PatoColorEditFlags flags = 0);
    Pato_API bool          ColorPicker4(const char* label, float col[4], PatoColorEditFlags flags = 0, const float* ref_col = NULL);
    Pato_API bool          ColorPicker(const char* label, ImColor* color);
    Pato_API bool          ColorButton(const char* desc_id, const ImVec4& col, PatoColorEditFlags flags = 0, ImVec2 size = ImVec2(0,0));  // display a colored square/button, hover for details, return true when pressed.
    Pato_API void          SetColorEditOptions(PatoColorEditFlags flags);                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

    // Widgets: Trees
    // TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
    Pato_API bool          TreeNode(const char* label);
    Pato_API bool          TreeNode(const char* str_id, const char* fmt, ...) IM_FMTARGS(2);   // helper variation to completely decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
    Pato_API bool          TreeNode(const void* ptr_id, const char* fmt, ...) IM_FMTARGS(2);   // "
    Pato_API bool          TreeNodeV(const char* str_id, const char* fmt, va_list args) IM_FMTLIST(2);
    Pato_API bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args) IM_FMTLIST(2);
    Pato_API bool          TreeNodeEx(const char* label, PatoTreeNodeFlags flags = 0);
    Pato_API bool          TreeNodeEx(const char* str_id, PatoTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    Pato_API bool          TreeNodeEx(const void* ptr_id, PatoTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    Pato_API bool          TreeNodeExV(const char* str_id, PatoTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    Pato_API bool          TreeNodeExV(const void* ptr_id, PatoTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    Pato_API void          TreePush(const char* str_id);                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
    Pato_API void          TreePush(const void* ptr_id = NULL);                                // "
    Pato_API void          TreePop();                                                          // ~ Unindent()+PopId()
    Pato_API void          TreeAdvanceToLabelPos();                                            // advance cursor x position by GetTreeNodeToLabelSpacing()
    Pato_API float         GetTreeNodeToLabelSpacing();                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
    Pato_API void          SetNextTreeNodeOpen(bool is_open, PatoCond cond = 0);              // set next TreeNode/CollapsingHeader open state.
    Pato_API bool          CollapsingHeader(const char* label, PatoTreeNodeFlags flags = 0);  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    Pato_API bool          CollapsingHeader(const char* label, bool* p_open, PatoTreeNodeFlags flags = 0); // when 'p_open' isn't NULL, display an additional small close button on upper right of the header

    // Widgets: Selectables
    Pato_API bool          Selectable(const char* label, bool selected = false, PatoSelectableFlags flags = 0, const ImVec2& size = ImVec2(0,0));  // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    Pato_API bool          Selectable(const char* label, bool* p_selected, PatoSelectableFlags flags = 0, const ImVec2& size = ImVec2(0,0));       // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

    // Widgets: List Boxes
    Pato_API bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    Pato_API bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
    Pato_API bool          ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0,0)); // use if you want to reimplement ListBox() will custom data or interactions. if the function return true, you can output elements then call ListBoxFooter() afterwards.
    Pato_API bool          ListBoxHeader(const char* label, int items_count, int height_in_items = -1); // "
    Pato_API void          ListBoxFooter();                                                    // terminate the scrolling region. only call ListBoxFooter() if ListBoxHeader() returned true!

    // Widgets: Data Plotting
    Pato_API void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    Pato_API void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    Pato_API void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    Pato_API void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));

    // Widgets: Value() Helpers. Simple shortcut to calling Text() with a format string. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the Pato namespace)
    Pato_API void          Value(const char* prefix, bool b);
    Pato_API void          Value(const char* prefix, int v);
    Pato_API void          Value(const char* prefix, unsigned int v);
    Pato_API void          Value(const char* prefix, float v, const char* float_format = NULL);

    // Widgets: Menus
    Pato_API bool          BeginMainMenuBar();                                                 // create and append to a full screen menu-bar.
    Pato_API void          EndMainMenuBar();                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
    Pato_API bool          BeginMenuBar();                                                     // append to menu-bar of current window (requires PatoWindowFlags_MenuBar flag set on parent window).
    Pato_API void          EndMenuBar();                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
    Pato_API bool          BeginMenu(const char* label, bool enabled = true);                  // create a sub-menu entry. only call EndMenu() if this returns true!
    Pato_API void          EndMenu();                                                          // only call EndMenu() if BeginMenu() returns true!
    Pato_API bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated. shortcuts are displayed for convenience but not processed by Pato at the moment
    Pato_API bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

    // Tooltips
    Pato_API void          BeginTooltip();                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
    Pato_API void          EndTooltip();
    Pato_API void          SetTooltip(const char* fmt, ...) IM_FMTARGS(1);                     // set a text-only tooltip, typically use with Pato::IsItemHovered(). overidde any previous call to SetTooltip().
    Pato_API void          SetTooltipV(const char* fmt, va_list args) IM_FMTLIST(1);

    // Popups, Modals
    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by Pato instead of being held by the programmer as we are used to with regular Begin() calls.
    //   User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(PatoHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.
    Pato_API void          OpenPopup(const char* str_id);                                      // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
    Pato_API bool          BeginPopup(const char* str_id, PatoWindowFlags flags = 0);                                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
    Pato_API bool          BeginPopupContextItem(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked on last item. if you can pass a NULL str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
    Pato_API bool          BeginPopupContextWindow(const char* str_id = NULL, int mouse_button = 1, bool also_over_items = true);  // helper to open and begin popup when clicked on current window.
    Pato_API bool          BeginPopupContextVoid(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked in void (where there are no Pato windows).
    Pato_API bool          BeginPopupModal(const char* name, bool* p_open = NULL, PatoWindowFlags flags = 0);                     // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
    Pato_API void          EndPopup();                                                                                             // only call EndPopup() if BeginPopupXXX() returns true!
    Pato_API bool          OpenPopupOnItemClick(const char* str_id = NULL, int mouse_button = 1);                                  // helper to open popup when clicked on last item (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors). return true when just opened.
    Pato_API bool          IsPopupOpen(const char* str_id);                                    // return true if the popup is open at the current begin-ed level of the popup stack.
    Pato_API void          CloseCurrentPopup();                                                // close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.

    // Columns
    // You can also use SameLine(pos_x) to mimic simplified columns. The columns API is work-in-progress and rather lacking (columns are arguably the worst part of dear Pato at the moment!)
    Pato_API void          Columns(int count = 1, const char* id = NULL, bool border = true);
    Pato_API void          NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished
    Pato_API int           GetColumnIndex();                                                   // get current column index
    Pato_API float         GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column
    Pato_API void          SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column
    Pato_API float         GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
    Pato_API void          SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
    Pato_API int           GetColumnsCount();

    // Tab Bars, Tabs
    // [BETA API] API may evolve!
    Pato_API bool          BeginTabBar(const char* str_id, PatoTabBarFlags flags = 0);        // create and append into a TabBar
    Pato_API void          EndTabBar();                                                        // only call EndTabBar() if BeginTabBar() returns true!
    Pato_API bool          BeginTabItem(const char* label, bool* p_open = NULL, PatoTabItemFlags flags = 0);// create a Tab. Returns true if the Tab is selected.
    Pato_API void          EndTabItem();                                                       // only call EndTabItem() if BeginTabItem() returns true!
    Pato_API void          SetTabItemClosed(const char* tab_or_docked_window_label);           // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

    // Logging/Capture: all text output from interface is captured to tty/file/clipboard. By default, tree nodes are automatically opened during logging.
    Pato_API void          LogToTTY(int max_depth = -1);                                       // start logging to tty (stdout)
    Pato_API void          LogToFile(int max_depth = -1, const char* filename = NULL);         // start logging to file
    Pato_API void          LogToClipboard(int max_depth = -1);                                 // start logging to OS clipboard
    Pato_API void          LogFinish();                                                        // stop logging (close file, etc.)
    Pato_API void          LogButtons();                                                       // helper to display buttons for logging to tty/file/clipboard
    Pato_API void          LogText(const char* fmt, ...) IM_FMTARGS(1);                        // pass text data straight to log (without being displayed)

    // Drag and Drop
    // [BETA API] Missing Demo code. API may evolve!
    Pato_API bool          BeginDragDropSource(PatoDragDropFlags flags = 0);                                      // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
    Pato_API bool          SetDragDropPayload(const char* type, const void* data, size_t size, PatoCond cond = 0);// type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear Pato internal types. Data is copied and held by Pato.
    Pato_API void          EndDragDropSource();                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
    Pato_API bool                  BeginDragDropTarget();                                                          // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
    Pato_API const PatoPayload*   AcceptDragDropPayload(const char* type, PatoDragDropFlags flags = 0);          // accept contents of a given type. If PatoDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
    Pato_API void                  EndDragDropTarget();                                                            // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
    Pato_API const PatoPayload*   GetDragDropPayload();                                                           // peek directly into the current payload from anywhere. may return NULL. use PatoPayload::IsDataType() to test for the payload type.
    
    // Clipping
    Pato_API void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    Pato_API void          PopClipRect();

    // Focus, Activation
    // Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
    Pato_API void          SetItemDefaultFocus();                                              // make last item the default focused item of a window.
    Pato_API void          SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

    // Item/Widgets Utilities
    // Most of the functions are referring to the last/previous item we submitted.
    // See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
    Pato_API bool          IsItemHovered(PatoHoveredFlags flags = 0);                         // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See PatoHoveredFlags for more options.
    Pato_API bool          IsItemActive();                                                     // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
    Pato_API bool          IsItemFocused();                                                    // is the last item focused for keyboard/gamepad navigation?
    Pato_API bool          IsItemClicked(int mouse_button = 0);                                // is the last item clicked? (e.g. button/node just clicked on) == IsMouseClicked(mouse_button) && IsItemHovered()
    Pato_API bool          IsItemVisible();                                                    // is the last item visible? (items may be out of sight because of clipping/scrolling)
    Pato_API bool          IsItemEdited();                                                     // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
    Pato_API bool          IsItemDeactivated();                                                // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
    Pato_API bool          IsItemDeactivatedAfterEdit();                                       // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
    Pato_API bool          IsAnyItemHovered();
    Pato_API bool          IsAnyItemActive();
    Pato_API bool          IsAnyItemFocused();
    Pato_API ImVec2        GetItemRectMin();                                                   // get bounding rectangle of last item, in screen space
    Pato_API ImVec2        GetItemRectMax();                                                   // "
    Pato_API ImVec2        GetItemRectSize();                                                  // get size of last item, in screen space
    Pato_API void          SetItemAllowOverlap();                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

    // Miscellaneous Utilities
    Pato_API bool          IsRectVisible(const ImVec2& size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
    Pato_API bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
    Pato_API double        GetTime();                                                          // get global Pato time. incremented by io.DeltaTime every frame.
    Pato_API int           GetFrameCount();                                                    // get global Pato frame count. incremented by 1 every frame.
    Pato_API ImDrawList*   GetOverlayDrawList();                                               // this draw list will be the last rendered one, useful to quickly draw overlays shapes/text
    Pato_API ImDrawListSharedData* GetDrawListSharedData();                                    // you may use this when creating your own ImDrawList instances
    Pato_API const char*   GetStyleColorName(PatoCol idx);
    Pato_API void          SetStateStorage(PatoStorage* storage);                             // replace current window storage with our own (if you want to manipulate it yourself, typically clear subsection of it)
    Pato_API PatoStorage* GetStateStorage();
    Pato_API ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
    Pato_API void          CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end);    // calculate coarse clipping for large list of evenly sized items. Prefer using the PatoListClipper higher-level helper if you can.
    Pato_API bool          BeginChildFrame(PatoID id, const ImVec2& size, PatoWindowFlags flags = 0); // helper to create a child window / scrolling region that looks like a normal widget frame
    Pato_API void          EndChildFrame();                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)

    // Color Utilities
    Pato_API ImVec4        ColorConvertU32ToFloat4(ImU32 in);
    Pato_API ImU32         ColorConvertFloat4ToU32(const ImVec4& in);
    Pato_API void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
    Pato_API void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    // Inputs Utilities
    Pato_API int           GetKeyIndex(PatoKey Pato_key);                                    // map PatoKey_* values into user's key index. == io.KeyMap[key]
    Pato_API bool          IsKeyDown(int user_key_index);                                      // is key being held. == io.KeysDown[user_key_index]. note that Pato doesn't know the semantic of each entry of io.KeysDown[]. Use your own indices/enums according to how your backend/engine stored them into io.KeysDown[]!
    Pato_API bool          IsKeyPressed(int user_key_index, bool repeat = true);               // was key pressed (went from !Down to Down). if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
    Pato_API bool          IsKeyReleased(int user_key_index);                                  // was key released (went from Down to !Down)..
    Pato_API int           GetKeyPressedAmount(int key_index, float repeat_delay, float rate); // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
    Pato_API bool          IsMouseDown(int button);                                            // is mouse button held (0=left, 1=right, 2=middle)
    Pato_API bool          IsAnyMouseDown();                                                   // is any mouse button held
    Pato_API bool          IsMouseClicked(int button, bool repeat = false);                    // did mouse button clicked (went from !Down to Down) (0=left, 1=right, 2=middle)
    Pato_API bool          IsMouseDoubleClicked(int button);                                   // did mouse button double-clicked. a double-click returns false in IsMouseClicked(). uses io.MouseDoubleClickTime.
    Pato_API bool          IsMouseReleased(int button);                                        // did mouse button released (went from Down to !Down)
    Pato_API bool          IsMouseDragging(int button = 0, float lock_threshold = -1.0f);      // is mouse dragging. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
    Pato_API bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);  // is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but  disregarding of other consideration of focus/window ordering/popup-block.
    Pato_API bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);                    //
    Pato_API ImVec2        GetMousePos();                                                      // shortcut to Pato::GetIO().MousePos provided by user, to be consistent with other calls
    Pato_API ImVec2        GetMousePosOnOpeningCurrentPopup();                                 // retrieve backup of mouse position at the time of opening popup we have BeginPopup() into
    Pato_API ImVec2        GetMouseDragDelta(int button = 0, float lock_threshold = -1.0f);    // dragging amount since clicking. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
    Pato_API void          ResetMouseDragDelta(int button = 0);                                //
    Pato_API PatoMouseCursor GetMouseCursor();                                                // get desired cursor type, reset in Pato::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor Pato will render those for you
    Pato_API void          SetMouseCursor(PatoMouseCursor type);                              // set desired cursor type
    Pato_API void          CaptureKeyboardFromApp(bool want_capture_keyboard_value = true);    // attention: misleading name! manually override io.WantCaptureKeyboard flag next frame (said flag is entirely left for your application to handle). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard_value"; after the next NewFrame() call. 
    Pato_API void          CaptureMouseFromApp(bool want_capture_mouse_value = true);          // attention: misleading name! manually override io.WantCaptureMouse flag next frame (said flag is entirely left for your application to handle). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse_value;" after the next NewFrame() call. 

    // Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
    Pato_API const char*   GetClipboardText();
    Pato_API void          SetClipboardText(const char* text);

    // Settings/.Ini Utilities
    // The disk functions are automatically called if io.IniFilename != NULL (default is "Pato.ini").
    // Set io.IniFilename to NULL to load/save manually. Read io.WantSaveIniSettings description about handling .ini saving manually.
    Pato_API void          LoadIniSettingsFromDisk(const char* ini_filename);                  // call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename).
    Pato_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size=0); // call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source.
    Pato_API void          SaveIniSettingsToDisk(const char* ini_filename);
    Pato_API const char*   SaveIniSettingsToMemory(size_t* out_ini_size = NULL);               // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

    // Memory Utilities
    // All those functions are not reliant on the current context.
    // If you reload the contents of Pato.cpp at runtime, you may need to call SetCurrentContext() + SetAllocatorFunctions() again.
    Pato_API void          SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void(*free_func)(void* ptr, void* user_data), void* user_data = NULL);
    Pato_API void*         MemAlloc(size_t size);
    Pato_API void          MemFree(void* ptr);

} // namespace Pato

//-----------------------------------------------------------------------------
// Flags & Enumerations
//-----------------------------------------------------------------------------

// Flags for Pato::Begin()
enum PatoWindowFlags_
{
    PatoWindowFlags_None                   = 0,
    PatoWindowFlags_NoTitleBar             = 1 << 0,   // Disable title-bar
    PatoWindowFlags_NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
    PatoWindowFlags_NoMove                 = 1 << 2,   // Disable user moving the window
    PatoWindowFlags_NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programatically)
    PatoWindowFlags_NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    PatoWindowFlags_NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it
    PatoWindowFlags_AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
    PatoWindowFlags_NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    PatoWindowFlags_NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
    PatoWindowFlags_NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    PatoWindowFlags_MenuBar                = 1 << 10,  // Has a menu-bar
    PatoWindowFlags_HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in Pato_demo in the "Horizontal Scrolling" section.
    PatoWindowFlags_NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    PatoWindowFlags_NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
    PatoWindowFlags_AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    PatoWindowFlags_AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    PatoWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    PatoWindowFlags_NoNavInputs            = 1 << 18,  // No gamepad/keyboard navigation within the window
    PatoWindowFlags_NoNavFocus             = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    PatoWindowFlags_UnsavedDocument        = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
    PatoWindowFlags_NoNav                  = PatoWindowFlags_NoNavInputs | PatoWindowFlags_NoNavFocus,
    PatoWindowFlags_NoDecoration           = PatoWindowFlags_NoTitleBar | PatoWindowFlags_NoResize | PatoWindowFlags_NoScrollbar | PatoWindowFlags_NoCollapse,
    PatoWindowFlags_NoInputs               = PatoWindowFlags_NoMouseInputs | PatoWindowFlags_NoNavInputs | PatoWindowFlags_NoNavFocus,

    // [Internal]
    PatoWindowFlags_NavFlattened           = 1 << 23,  // [BETA] Allow gamepad/keyboard navigation to cross over parent border to this child (only use on child that have no scrolling!)
    PatoWindowFlags_ChildWindow            = 1 << 24,  // Don't use! For internal use by BeginChild()
    PatoWindowFlags_Tooltip                = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    PatoWindowFlags_Popup                  = 1 << 26,  // Don't use! For internal use by BeginPopup()
    PatoWindowFlags_Modal                  = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    PatoWindowFlags_ChildMenu              = 1 << 28   // Don't use! For internal use by BeginMenu()

    // [Obsolete]
    //PatoWindowFlags_ShowBorders          = 1 << 7,   // --> Set style.FrameBorderSize=1.0f / style.WindowBorderSize=1.0f to enable borders around windows and items
    //PatoWindowFlags_ResizeFromAnySide    = 1 << 17,  // --> Set io.ConfigWindowsResizeFromEdges and make sure mouse cursors are supported by back-end (io.BackendFlags & PatoBackendFlags_HasMouseCursors)
};

// Flags for Pato::InputText()
enum PatoInputTextFlags_
{
    PatoInputTextFlags_None                = 0,
    PatoInputTextFlags_CharsDecimal        = 1 << 0,   // Allow 0123456789.+-*/
    PatoInputTextFlags_CharsHexadecimal    = 1 << 1,   // Allow 0123456789ABCDEFabcdef
    PatoInputTextFlags_CharsUppercase      = 1 << 2,   // Turn a..z into A..Z
    PatoInputTextFlags_CharsNoBlank        = 1 << 3,   // Filter out spaces, tabs
    PatoInputTextFlags_AutoSelectAll       = 1 << 4,   // Select entire text when first taking mouse focus
    PatoInputTextFlags_EnterReturnsTrue    = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to when the value was modified)
    PatoInputTextFlags_CallbackCompletion  = 1 << 6,   // Callback on pressing TAB (for completion handling)
    PatoInputTextFlags_CallbackHistory     = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
    PatoInputTextFlags_CallbackAlways      = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
    PatoInputTextFlags_CallbackCharFilter  = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
    PatoInputTextFlags_AllowTabInput       = 1 << 10,  // Pressing TAB input a '\t' character into the text field
    PatoInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
    PatoInputTextFlags_NoHorizontalScroll  = 1 << 12,  // Disable following the cursor horizontally
    PatoInputTextFlags_AlwaysInsertMode    = 1 << 13,  // Insert mode
    PatoInputTextFlags_ReadOnly            = 1 << 14,  // Read-only mode
    PatoInputTextFlags_Password            = 1 << 15,  // Password mode, display all characters as '*'
    PatoInputTextFlags_NoUndoRedo          = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
    PatoInputTextFlags_CharsScientific     = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
    PatoInputTextFlags_CallbackResize      = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/Pato_stdlib.h for an example of using this)
    // [Internal]
    PatoInputTextFlags_Multiline           = 1 << 20   // For internal use by InputTextMultiline()
};

// Flags for Pato::TreeNodeEx(), Pato::CollapsingHeader*()
enum PatoTreeNodeFlags_
{
    PatoTreeNodeFlags_None                 = 0,
    PatoTreeNodeFlags_Selected             = 1 << 0,   // Draw as selected
    PatoTreeNodeFlags_Framed               = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
    PatoTreeNodeFlags_AllowItemOverlap     = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
    PatoTreeNodeFlags_NoTreePushOnOpen     = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    PatoTreeNodeFlags_NoAutoOpenOnLog      = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    PatoTreeNodeFlags_DefaultOpen          = 1 << 5,   // Default node to be open
    PatoTreeNodeFlags_OpenOnDoubleClick    = 1 << 6,   // Need double-click to open node
    PatoTreeNodeFlags_OpenOnArrow          = 1 << 7,   // Only open when clicking on the arrow part. If PatoTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    PatoTreeNodeFlags_Leaf                 = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
    PatoTreeNodeFlags_Bullet               = 1 << 9,   // Display a bullet instead of arrow
    PatoTreeNodeFlags_FramePadding         = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
    //PatoTreeNodeFlags_SpanAllAvailWidth  = 1 << 11,  // FIXME: TODO: Extend hit box horizontally even if not framed
    //PatoTreeNodeFlags_NoScrollOnOpen     = 1 << 12,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
    PatoTreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
    PatoTreeNodeFlags_CollapsingHeader     = PatoTreeNodeFlags_Framed | PatoTreeNodeFlags_NoTreePushOnOpen | PatoTreeNodeFlags_NoAutoOpenOnLog

    // Obsolete names (will be removed)
#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    , PatoTreeNodeFlags_AllowOverlapMode = PatoTreeNodeFlags_AllowItemOverlap
#endif
};

// Flags for Pato::Selectable()
enum PatoSelectableFlags_
{
    PatoSelectableFlags_None               = 0,
    PatoSelectableFlags_DontClosePopups    = 1 << 0,   // Clicking this don't close parent popup window
    PatoSelectableFlags_SpanAllColumns     = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
    PatoSelectableFlags_AllowDoubleClick   = 1 << 2,   // Generate press events on double clicks too
    PatoSelectableFlags_Disabled           = 1 << 3    // Cannot be selected, display greyed out text
};

// Flags for Pato::BeginCombo()
enum PatoComboFlags_
{
    PatoComboFlags_None                    = 0,
    PatoComboFlags_PopupAlignLeft          = 1 << 0,   // Align the popup toward the left by default
    PatoComboFlags_HeightSmall             = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
    PatoComboFlags_HeightRegular           = 1 << 2,   // Max ~8 items visible (default)
    PatoComboFlags_HeightLarge             = 1 << 3,   // Max ~20 items visible
    PatoComboFlags_HeightLargest           = 1 << 4,   // As many fitting items as possible
    PatoComboFlags_NoArrowButton           = 1 << 5,   // Display on the preview box without the square arrow button
    PatoComboFlags_NoPreview               = 1 << 6,   // Display only a square arrow button
    PatoComboFlags_HeightMask_             = PatoComboFlags_HeightSmall | PatoComboFlags_HeightRegular | PatoComboFlags_HeightLarge | PatoComboFlags_HeightLargest
};

// Flags for Pato::BeginTabBar()
enum PatoTabBarFlags_
{
    PatoTabBarFlags_None                           = 0,
    PatoTabBarFlags_Reorderable                    = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
    PatoTabBarFlags_AutoSelectNewTabs              = 1 << 1,   // Automatically select new tabs when they appear
    PatoTabBarFlags_NoCloseWithMiddleMouseButton   = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    PatoTabBarFlags_NoTabListPopupButton           = 1 << 3,
    PatoTabBarFlags_NoTabListScrollingButtons      = 1 << 4,
    PatoTabBarFlags_FittingPolicyResizeDown        = 1 << 5,   // Resize tabs when they don't fit
    PatoTabBarFlags_FittingPolicyScroll            = 1 << 6,   // Add scroll buttons when tabs don't fit
    PatoTabBarFlags_FittingPolicyMask_             = PatoTabBarFlags_FittingPolicyResizeDown | PatoTabBarFlags_FittingPolicyScroll,
    PatoTabBarFlags_FittingPolicyDefault_          = PatoTabBarFlags_FittingPolicyResizeDown
};  

// Flags for Pato::BeginTabItem()
enum PatoTabItemFlags_
{
    PatoTabItemFlags_None                          = 0,
    PatoTabItemFlags_UnsavedDocument               = 1 << 0,   // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. Also: tab is selected on closure and closure is deferred by one frame to allow code to undo it without flicker.
    PatoTabItemFlags_SetSelected                   = 1 << 1,   // Trigger flag to programatically make the tab selected when calling BeginTabItem()
    PatoTabItemFlags_NoCloseWithMiddleMouseButton  = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    PatoTabItemFlags_NoPushId                      = 1 << 3    // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
};

// Flags for Pato::IsWindowFocused()
enum PatoFocusedFlags_
{
    PatoFocusedFlags_None                          = 0,
    PatoFocusedFlags_ChildWindows                  = 1 << 0,   // IsWindowFocused(): Return true if any children of the window is focused
    PatoFocusedFlags_RootWindow                    = 1 << 1,   // IsWindowFocused(): Test from root window (top most parent of the current hierarchy)
    PatoFocusedFlags_AnyWindow                     = 1 << 2,   // IsWindowFocused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use Pato::GetIO().WantCaptureMouse instead.
    PatoFocusedFlags_RootAndChildWindows           = PatoFocusedFlags_RootWindow | PatoFocusedFlags_ChildWindows
};

// Flags for Pato::IsItemHovered(), Pato::IsWindowHovered()
// Note: if you are trying to check whether your mouse should be dispatched to Pato or to your app, you should use the 'io.WantCaptureMouse' boolean for that. Please read the FAQ!
// Note: windows with the PatoWindowFlags_NoInputs flag are ignored by IsWindowHovered() calls.
enum PatoHoveredFlags_
{
    PatoHoveredFlags_None                          = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
    PatoHoveredFlags_ChildWindows                  = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
    PatoHoveredFlags_RootWindow                    = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
    PatoHoveredFlags_AnyWindow                     = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
    PatoHoveredFlags_AllowWhenBlockedByPopup       = 1 << 3,   // Return true even if a popup window is normally blocking access to this item/window
    //PatoHoveredFlags_AllowWhenBlockedByModal     = 1 << 4,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
    PatoHoveredFlags_AllowWhenBlockedByActiveItem  = 1 << 5,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
    PatoHoveredFlags_AllowWhenOverlapped           = 1 << 6,   // Return true even if the position is overlapped by another window
    PatoHoveredFlags_AllowWhenDisabled             = 1 << 7,   // Return true even if the item is disabled
    PatoHoveredFlags_RectOnly                      = PatoHoveredFlags_AllowWhenBlockedByPopup | PatoHoveredFlags_AllowWhenBlockedByActiveItem | PatoHoveredFlags_AllowWhenOverlapped,
    PatoHoveredFlags_RootAndChildWindows           = PatoHoveredFlags_RootWindow | PatoHoveredFlags_ChildWindows
};

// Flags for Pato::BeginDragDropSource(), Pato::AcceptDragDropPayload()
enum PatoDragDropFlags_
{
    PatoDragDropFlags_None                         = 0,
    // BeginDragDropSource() flags
    PatoDragDropFlags_SourceNoPreviewTooltip       = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
    PatoDragDropFlags_SourceNoDisableHover         = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
    PatoDragDropFlags_SourceNoHoldToOpenOthers     = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
    PatoDragDropFlags_SourceAllowNullID            = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear Pato ecosystem and so we made it explicit.
    PatoDragDropFlags_SourceExtern                 = 1 << 4,   // External source (from outside of Pato), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
    PatoDragDropFlags_SourceAutoExpirePayload      = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
    // AcceptDragDropPayload() flags
    PatoDragDropFlags_AcceptBeforeDelivery         = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
    PatoDragDropFlags_AcceptNoDrawDefaultRect      = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
    PatoDragDropFlags_AcceptNoPreviewTooltip       = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
    PatoDragDropFlags_AcceptPeekOnly               = PatoDragDropFlags_AcceptBeforeDelivery | PatoDragDropFlags_AcceptNoDrawDefaultRect  // For peeking ahead and inspecting the payload before delivery.
};

// Standard Drag and Drop payload types. You can define you own payload types using short strings. Types starting with '_' are defined by Dear Pato.
#define Pato_PAYLOAD_TYPE_COLOR_3F     "_COL3F"    // float[3]: Standard type for colors, without alpha. User code may use this type.
#define Pato_PAYLOAD_TYPE_COLOR_4F     "_COL4F"    // float[4]: Standard type for colors. User code may use this type.

// A primary data type
enum PatoDataType_
{
    PatoDataType_S32,      // int
    PatoDataType_U32,      // unsigned int
    PatoDataType_S64,      // long long, __int64
    PatoDataType_U64,      // unsigned long long, unsigned __int64
    PatoDataType_Float,    // float
    PatoDataType_Double,   // double
    PatoDataType_COUNT
};

// A cardinal direction
enum PatoDir_
{
    PatoDir_None    = -1,
    PatoDir_Left    = 0,
    PatoDir_Right   = 1,
    PatoDir_Up      = 2,
    PatoDir_Down    = 3,
    PatoDir_COUNT
};

// User fill PatoIO.KeyMap[] array with indices into the PatoIO.KeysDown[512] array
enum PatoKey_
{
    PatoKey_Tab,
    PatoKey_LeftArrow,
    PatoKey_RightArrow,
    PatoKey_UpArrow,
    PatoKey_DownArrow,
    PatoKey_PageUp,
    PatoKey_PageDown,
    PatoKey_Home,
    PatoKey_End,
    PatoKey_Insert,
    PatoKey_Delete,
    PatoKey_Backspace,
    PatoKey_Space,
    PatoKey_Enter,
    PatoKey_Escape,
    PatoKey_A,         // for text edit CTRL+A: select all
    PatoKey_C,         // for text edit CTRL+C: copy
    PatoKey_V,         // for text edit CTRL+V: paste
    PatoKey_X,         // for text edit CTRL+X: cut
    PatoKey_Y,         // for text edit CTRL+Y: redo
    PatoKey_Z,         // for text edit CTRL+Z: undo
    PatoKey_COUNT
};

// Gamepad/Keyboard directional navigation
// Keyboard: Set io.ConfigFlags |= PatoConfigFlags_NavEnableKeyboard to enable. NewFrame() will automatically fill io.NavInputs[] based on your io.KeysDown[] + io.KeyMap[] arrays.
// Gamepad:  Set io.ConfigFlags |= PatoConfigFlags_NavEnableGamepad to enable. Back-end: set PatoBackendFlags_HasGamepad and fill the io.NavInputs[] fields before calling NewFrame(). Note that io.NavInputs[] is cleared by EndFrame().
// Read instructions in Pato.cpp for more details. Download PNG/PSD at http://goo.gl/9LgVZW.
enum PatoNavInput_
{
    // Gamepad Mapping
    PatoNavInput_Activate,      // activate / open / toggle / tweak value       // e.g. Cross  (PS4), A (Xbox), A (Switch), Space (Keyboard)
    PatoNavInput_Cancel,        // cancel / close / exit                        // e.g. Circle (PS4), B (Xbox), B (Switch), Escape (Keyboard)
    PatoNavInput_Input,         // text input / on-screen keyboard              // e.g. Triang.(PS4), Y (Xbox), X (Switch), Return (Keyboard)
    PatoNavInput_Menu,          // tap: toggle menu / hold: focus, move, resize // e.g. Square (PS4), X (Xbox), Y (Switch), Alt (Keyboard)
    PatoNavInput_DpadLeft,      // move / tweak / resize window (w/ PadMenu)    // e.g. D-pad Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
    PatoNavInput_DpadRight,     //
    PatoNavInput_DpadUp,        //
    PatoNavInput_DpadDown,      //
    PatoNavInput_LStickLeft,    // scroll / move window (w/ PadMenu)            // e.g. Left Analog Stick Left/Right/Up/Down
    PatoNavInput_LStickRight,   //
    PatoNavInput_LStickUp,      //
    PatoNavInput_LStickDown,    //
    PatoNavInput_FocusPrev,     // next window (w/ PadMenu)                     // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    PatoNavInput_FocusNext,     // prev window (w/ PadMenu)                     // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)
    PatoNavInput_TweakSlow,     // slower tweaks                                // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    PatoNavInput_TweakFast,     // faster tweaks                                // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)

    // [Internal] Don't use directly! This is used internally to differentiate keyboard from gamepad inputs for behaviors that require to differentiate them.
    // Keyboard behavior that have no corresponding gamepad mapping (e.g. CTRL+TAB) will be directly reading from io.KeysDown[] instead of io.NavInputs[].
    PatoNavInput_KeyMenu_,      // toggle menu                                  // = io.KeyAlt
    PatoNavInput_KeyLeft_,      // move left                                    // = Arrow keys
    PatoNavInput_KeyRight_,     // move right
    PatoNavInput_KeyUp_,        // move up
    PatoNavInput_KeyDown_,      // move down
    PatoNavInput_COUNT,
    PatoNavInput_InternalLoopCallback_ = PatoNavInput_KeyMenu_
};

// Configuration flags stored in io.ConfigFlags. Set by user/application.
enum PatoConfigFlags_
{
    PatoConfigFlags_None                   = 0,
    PatoConfigFlags_NavEnableKeyboard      = 1 << 0,   // Master keyboard navigation enable flag. NewFrame() will automatically fill io.NavInputs[] based on io.KeysDown[].
    PatoConfigFlags_NavEnableGamepad       = 1 << 1,   // Master gamepad navigation enable flag. This is mostly to instruct your Pato back-end to fill io.NavInputs[]. Back-end also needs to set PatoBackendFlags_HasGamepad.
    PatoConfigFlags_NavEnableSetMousePos   = 1 << 2,   // Instruct navigation to move the mouse cursor. May be useful on TV/console systems where moving a virtual mouse is awkward. Will update io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor io.WantSetMousePos requests in your binding, otherwise Pato will react as if the mouse is jumping around back and forth.
    PatoConfigFlags_NavNoCaptureKeyboard   = 1 << 3,   // Instruct navigation to not set the io.WantCaptureKeyboard flag when io.NavActive is set.
    PatoConfigFlags_NoMouse                = 1 << 4,   // Instruct Pato to clear mouse position/buttons in NewFrame(). This allows ignoring the mouse information set by the back-end.
    PatoConfigFlags_NoMouseCursorChange    = 1 << 5,   // Instruct back-end to not alter mouse cursor shape and visibility. Use if the back-end cursor changes are interfering with yours and you don't want to use SetMouseCursor() to change mouse cursor. You may want to honor requests from Pato by reading GetMouseCursor() yourself instead.

    // User storage (to allow your back-end/engine to communicate to code that may be shared between multiple projects. Those flags are not used by core Pato)
    PatoConfigFlags_IsSRGB                 = 1 << 20,  // Application is SRGB-aware.
    PatoConfigFlags_IsTouchScreen          = 1 << 21   // Application is using a touch screen instead of a mouse.
};

// Back-end capabilities flags stored in io.BackendFlags. Set by Pato_impl_xxx or custom back-end.
enum PatoBackendFlags_
{
    PatoBackendFlags_None                  = 0,
    PatoBackendFlags_HasGamepad            = 1 << 0,   // Back-end supports gamepad and currently has one connected.
    PatoBackendFlags_HasMouseCursors       = 1 << 1,   // Back-end supports honoring GetMouseCursor() value to change the OS cursor shape.
    PatoBackendFlags_HasSetMousePos        = 1 << 2    // Back-end supports io.WantSetMousePos requests to reposition the OS mouse position (only used if PatoConfigFlags_NavEnableSetMousePos is set).
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum PatoCol_
{
    PatoCol_Text,
    PatoCol_TextDisabled,
    PatoCol_WindowBg,              // Background of normal windows
    PatoCol_ChildBg,               // Background of child windows
    PatoCol_PopupBg,               // Background of popups, menus, tooltips windows
    PatoCol_Border,
    PatoCol_BorderShadow,
    PatoCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
    PatoCol_FrameBgHovered,
    PatoCol_FrameBgActive,
    PatoCol_TitleBg,
    PatoCol_TitleBgActive,
    PatoCol_TitleBgCollapsed,
    PatoCol_MenuBarBg,
    PatoCol_ScrollbarBg,
    PatoCol_ScrollbarGrab,
    PatoCol_ScrollbarGrabHovered,
    PatoCol_ScrollbarGrabActive,
    PatoCol_CheckMark,
    PatoCol_SliderGrab,
    PatoCol_SliderGrabActive,
    PatoCol_Button,
    PatoCol_ButtonHovered,
    PatoCol_ButtonActive,
    PatoCol_Header,
    PatoCol_HeaderHovered,
    PatoCol_HeaderActive,
    PatoCol_Separator,
    PatoCol_SeparatorHovered,
    PatoCol_SeparatorActive,
    PatoCol_ResizeGrip,
    PatoCol_ResizeGripHovered,
    PatoCol_ResizeGripActive,
    PatoCol_Tab,
    PatoCol_TabHovered,
    PatoCol_TabActive,
    PatoCol_TabUnfocused,
    PatoCol_TabUnfocusedActive,
    PatoCol_PlotLines,
    PatoCol_PlotLinesHovered,
    PatoCol_PlotHistogram,
    PatoCol_PlotHistogramHovered,
    PatoCol_TextSelectedBg,
    PatoCol_DragDropTarget,
    PatoCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
    PatoCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
    PatoCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
    PatoCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
    PatoCol_COUNT

    // Obsolete names (will be removed)
#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    , PatoCol_ChildWindowBg = PatoCol_ChildBg, PatoCol_Column = PatoCol_Separator, PatoCol_ColumnHovered = PatoCol_SeparatorHovered, PatoCol_ColumnActive = PatoCol_SeparatorActive
    , PatoCol_ModalWindowDarkening = PatoCol_ModalWindowDimBg
    //PatoCol_CloseButton, PatoCol_CloseButtonActive, PatoCol_CloseButtonHovered, // [unused since 1.60+] the close button now uses regular button colors.
    //PatoCol_ComboBg,                                                              // [unused since 1.53+] ComboBg has been merged with PopupBg, so a redirect isn't accurate.
#endif
};

// Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify the PatoStyle structure.
// NB: the enum only refers to fields of PatoStyle which makes sense to be pushed/popped inside UI code. During initialization, feel free to just poke into PatoStyle directly.
// NB: if changing this enum, you need to update the associated internal table GStyleVarInfo[] accordingly. This is where we link enum values to members offset/type.
enum PatoStyleVar_
{
    // Enum name ......................// Member in PatoStyle structure (see PatoStyle for descriptions)
    PatoStyleVar_Alpha,               // float     Alpha
    PatoStyleVar_WindowPadding,       // ImVec2    WindowPadding
    PatoStyleVar_WindowRounding,      // float     WindowRounding
    PatoStyleVar_WindowBorderSize,    // float     WindowBorderSize
    PatoStyleVar_WindowMinSize,       // ImVec2    WindowMinSize
    PatoStyleVar_WindowTitleAlign,    // ImVec2    WindowTitleAlign
    PatoStyleVar_ChildRounding,       // float     ChildRounding
    PatoStyleVar_ChildBorderSize,     // float     ChildBorderSize
    PatoStyleVar_PopupRounding,       // float     PopupRounding
    PatoStyleVar_PopupBorderSize,     // float     PopupBorderSize
    PatoStyleVar_FramePadding,        // ImVec2    FramePadding
    PatoStyleVar_FrameRounding,       // float     FrameRounding
    PatoStyleVar_FrameBorderSize,     // float     FrameBorderSize
    PatoStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
    PatoStyleVar_ItemInnerSpacing,    // ImVec2    ItemInnerSpacing
    PatoStyleVar_IndentSpacing,       // float     IndentSpacing
    PatoStyleVar_ScrollbarSize,       // float     ScrollbarSize
    PatoStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
    PatoStyleVar_GrabMinSize,         // float     GrabMinSize
    PatoStyleVar_GrabRounding,        // float     GrabRounding
    PatoStyleVar_TabRounding,         // float     TabRounding
    PatoStyleVar_ButtonTextAlign,     // ImVec2    ButtonTextAlign
    PatoStyleVar_COUNT

    // Obsolete names (will be removed)
#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    , PatoStyleVar_Count_ = PatoStyleVar_COUNT, PatoStyleVar_ChildWindowRounding = PatoStyleVar_ChildRounding
#endif
};

// Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
enum PatoColorEditFlags_
{
    PatoColorEditFlags_None            = 0,
    PatoColorEditFlags_NoAlpha         = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (read 3 components from the input pointer).
    PatoColorEditFlags_NoPicker        = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
    PatoColorEditFlags_NoOptions       = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
    PatoColorEditFlags_NoSmallPreview  = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
    PatoColorEditFlags_NoInputs        = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
    PatoColorEditFlags_NoTooltip       = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
    PatoColorEditFlags_NoLabel         = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
    PatoColorEditFlags_NoSidePreview   = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
    PatoColorEditFlags_NoDragDrop      = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

    // User Options (right-click on widget to change some of them). You can set application defaults using SetColorEditOptions(). The idea is that you probably don't want to override them in most of your calls, let the user choose and/or call SetColorEditOptions() during startup.
    PatoColorEditFlags_AlphaBar        = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
    PatoColorEditFlags_AlphaPreview    = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
    PatoColorEditFlags_AlphaPreviewHalf= 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
    PatoColorEditFlags_HDR             = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use PatoColorEditFlags_Float flag as well).
    PatoColorEditFlags_RGB             = 1 << 20,  // [Inputs]     // ColorEdit: choose one among RGB/HSV/HEX. ColorPicker: choose any combination using RGB/HSV/HEX.
    PatoColorEditFlags_HSV             = 1 << 21,  // [Inputs]     // "
    PatoColorEditFlags_HEX             = 1 << 22,  // [Inputs]     // "
    PatoColorEditFlags_Uint8           = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
    PatoColorEditFlags_Float           = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
    PatoColorEditFlags_PickerHueBar    = 1 << 25,  // [PickerMode] // ColorPicker: bar for Hue, rectangle for Sat/Value.
    PatoColorEditFlags_PickerHueWheel  = 1 << 26,  // [PickerMode] // ColorPicker: wheel for Hue, triangle for Sat/Value.

    // [Internal] Masks
    PatoColorEditFlags__InputsMask     = PatoColorEditFlags_RGB|PatoColorEditFlags_HSV|PatoColorEditFlags_HEX,
    PatoColorEditFlags__DataTypeMask   = PatoColorEditFlags_Uint8|PatoColorEditFlags_Float,
    PatoColorEditFlags__PickerMask     = PatoColorEditFlags_PickerHueWheel|PatoColorEditFlags_PickerHueBar,
    PatoColorEditFlags__OptionsDefault = PatoColorEditFlags_Uint8|PatoColorEditFlags_RGB|PatoColorEditFlags_PickerHueBar    // Change application default using SetColorEditOptions()
};

// Enumeration for GetMouseCursor()
// User code may request binding to display given cursor by calling SetMouseCursor(), which is why we have some cursors that are marked unused here
enum PatoMouseCursor_
{
    PatoMouseCursor_None = -1,
    PatoMouseCursor_Arrow = 0,
    PatoMouseCursor_TextInput,         // When hovering over InputText, etc.
    PatoMouseCursor_ResizeAll,         // (Unused by Pato functions)
    PatoMouseCursor_ResizeNS,          // When hovering over an horizontal border
    PatoMouseCursor_ResizeEW,          // When hovering over a vertical border or a column
    PatoMouseCursor_ResizeNESW,        // When hovering over the bottom-left corner of a window
    PatoMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
    PatoMouseCursor_Hand,              // (Unused by Pato functions. Use for e.g. hyperlinks)
    PatoMouseCursor_COUNT

    // Obsolete names (will be removed)
#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    , PatoMouseCursor_Count_ = PatoMouseCursor_COUNT
#endif
};

// Enumateration for Pato::SetWindow***(), SetNextWindow***(), SetNextTreeNode***() functions
// Represent a condition.
// Important: Treat as a regular enum! Do NOT combine multiple values using binary operators! All the functions above treat 0 as a shortcut to PatoCond_Always.
enum PatoCond_
{
    PatoCond_Always        = 1 << 0,   // Set the variable
    PatoCond_Once          = 1 << 1,   // Set the variable once per runtime session (only the first call with succeed)
    PatoCond_FirstUseEver  = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    PatoCond_Appearing     = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)

    // Obsolete names (will be removed)
#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    , PatoSetCond_Always = PatoCond_Always, PatoSetCond_Once = PatoCond_Once, PatoSetCond_FirstUseEver = PatoCond_FirstUseEver, PatoSetCond_Appearing = PatoCond_Appearing
#endif
};

//-----------------------------------------------------------------------------
// PatoStyle
// You may modify the Pato::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use Pato::PushStyleVar(PatoStyleVar_XXXX)/PopStyleVar() to alter the main style values, 
// and Pato::PushStyleColor(PatoCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------

struct PatoStyle
{
    float       Alpha;                      // Global alpha applies to everything in Pato.
    ImVec2      WindowPadding;              // Padding within a window.
    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
    float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
    ImVec2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
    float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
    float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
    float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
    ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns.
    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
    float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    float       TabBorderSize;              // Thickness of border around tabs. 
    ImVec2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f,0.5f) for horizontally+vertically centered.
    ImVec2      DisplayWindowPadding;       // Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
    ImVec2      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
    float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
    bool        AntiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    ImVec4      Colors[PatoCol_COUNT];

    Pato_API PatoStyle();
    Pato_API void ScaleAllSizes(float scale_factor);
};

//-----------------------------------------------------------------------------
// PatoIO
// Communicate most settings and inputs/outputs to Dear Pato using this structure. 
// Access via Pato::GetIO(). Read 'Programmer guide' section in .cpp file for general usage.
//-----------------------------------------------------------------------------

struct PatoIO
{
    //------------------------------------------------------------------
    // Configuration (fill once)            // Default value:
    //------------------------------------------------------------------

    PatoConfigFlags   ConfigFlags;         // = 0                  // See PatoConfigFlags_ enum. Set by user/application. Gamepad/keyboard navigation options, etc.
    PatoBackendFlags  BackendFlags;        // = 0                  // See PatoBackendFlags_ enum. Set by back-end (Pato_impl_xxx files or custom back-end) to communicate features supported by the back-end.
    ImVec2        DisplaySize;              // <unset>              // Main display size, in pixels. For clamping windows positions.
    float         DeltaTime;                // = 1.0f/60.0f         // Time elapsed since last frame, in seconds.
    float         IniSavingRate;            // = 5.0f               // Minimum time between saving positions/sizes to .ini file, in seconds.
    const char*   IniFilename;              // = "Pato.ini"        // Path to .ini file. Set NULL to disable automatic .ini loading/saving, if e.g. you want to manually load/save from memory.
    const char*   LogFilename;              // = "Pato_log.txt"    // Path to .log file (default parameter to Pato::LogToFile when no file is specified).
    float         MouseDoubleClickTime;     // = 0.30f              // Time for a double-click, in seconds.
    float         MouseDoubleClickMaxDist;  // = 6.0f               // Distance threshold to stay in to validate a double-click, in pixels.
    float         MouseDragThreshold;       // = 6.0f               // Distance threshold before considering we are dragging.
    int           KeyMap[PatoKey_COUNT];   // <unset>              // Map of indices into the KeysDown[512] entries array which represent your "native" keyboard state.
    float         KeyRepeatDelay;           // = 0.250f             // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
    float         KeyRepeatRate;            // = 0.050f             // When holding a key/button, rate at which it repeats, in seconds.
    void*         UserData;                 // = NULL               // Store your own data for retrieval by callbacks.

    ImFontAtlas*  Fonts;                    // <auto>               // Load and assemble one or more fonts into a single tightly packed texture. Output to Fonts array.
    float         FontGlobalScale;          // = 1.0f               // Global scale all fonts
    bool          FontAllowUserScaling;     // = false              // Allow user scaling text of individual window with CTRL+Wheel.
    ImFont*       FontDefault;              // = NULL               // Font to use on NewFrame(). Use NULL to uses Fonts->Fonts[0].
    ImVec2        DisplayFramebufferScale;  // = (1.0f,1.0f)        // For retina display or other situations where window coordinates are different from framebuffer coordinates. User storage only, presently not used by Pato.
    ImVec2        DisplayVisibleMin;        // <unset> (0.0f,0.0f)  // [OBSOLETE] If you use DisplaySize as a virtual space larger than your screen, set DisplayVisibleMin/Max to the visible area.
    ImVec2        DisplayVisibleMax;        // <unset> (0.0f,0.0f)  // [OBSOLETE: just use io.DisplaySize!] If the values are the same, we defaults to Min=(0.0f) and Max=DisplaySize

    // Miscellaneous configuration options
    bool          MouseDrawCursor;                  // = false      // Request Pato to draw a mouse cursor for you (if you are on a platform without a mouse cursor). Cannot be easily renamed to 'io.ConfigXXX' because this is frequently used by back-end implementations.
    bool          ConfigMacOSXBehaviors;            // = defined(__APPLE__) // OS X style: Text editing cursor movement using Alt instead of Ctrl, Shortcuts using Cmd/Super instead of Ctrl, Line/Text LoopCallback and End using Cmd+Arrows instead of Home/End, Double click selects by word instead of selecting whole text, Multi-selection in lists uses Cmd/Super instead of Ctrl (was called io.OptMacOSXBehaviors prior to 1.63)
    bool          ConfigInputTextCursorBlink;       // = true       // Set to false to disable blinking cursor, for users who consider it distracting. (was called: io.OptCursorBlink prior to 1.63)
    bool          ConfigWindowsResizeFromEdges;     // = true       // Enable resizing of windows from their edges and from the lower-left corner. This requires (io.BackendFlags & PatoBackendFlags_HasMouseCursors) because it needs mouse cursor feedback. (This used to be the a per-window PatoWindowFlags_ResizeFromAnySide flag)
    bool          ConfigWindowsMoveFromTitleBarOnly;// = false      // [BETA] Set to true to only allow moving windows when clicked+dragged from the title bar. Windows without a title bar are not affected.

    //------------------------------------------------------------------
    // Platform Functions
    // (the Pato_impl_xxxx back-end files are setting those up for you)
    //------------------------------------------------------------------

    // Optional: Platform/Renderer back-end name (informational only! will be displayed in About Window)
    const char* BackendPlatformName;
    const char* BackendRendererName;

    // Optional: Access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*GetClipboardTextFn)(void* user_data);
    void        (*SetClipboardTextFn)(void* user_data, const char* text);
    void*       ClipboardUserData;

    // Optional: Notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME on Windows)
    // (default to use native imm32 api on Windows)
    void        (*ImeSetInputScreenPosFn)(int x, int y);
    void*       ImeWindowHandle;                // (Windows) Set this to your HWND to get automatic IME cursor positioning.

#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    // [OBSOLETE since 1.60+] Rendering function, will be automatically called in Render(). Please call your rendering function yourself now!
    // You can obtain the ImDrawData* by calling Pato::GetDrawData() after Render(). See example applications if you are unsure of how to implement this.
    void        (*RenderDrawListsFn)(ImDrawData* data);
#else
    // This is only here to keep PatoIO the same size, so that Pato_DISABLE_OBSOLETE_FUNCTIONS can exceptionally be used outside of imconfig.h.
    void*       RenderDrawListsFnUnused;
#endif

    //------------------------------------------------------------------
    // Input - Fill before calling NewFrame()
    //------------------------------------------------------------------

    ImVec2      MousePos;                       // Mouse position, in pixels. Set to ImVec2(-FLT_MAX,-FLT_MAX) if mouse is unavailable (on another screen, etc.)
    bool        MouseDown[5];                   // Mouse buttons: 0=left, 1=right, 2=middle + extras. Pato itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
    float       MouseWheel;                     // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
    float       MouseWheelH;                    // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all back-ends.
    bool        KeyCtrl;                        // Keyboard modifier pressed: Control
    bool        KeyShift;                       // Keyboard modifier pressed: Shift
    bool        KeyAlt;                         // Keyboard modifier pressed: Alt
    bool        KeySuper;                       // Keyboard modifier pressed: Cmd/Super/Windows
    bool        KeysDown[512];                  // Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys).
    ImWchar     InputCharacters[16+1];          // List of characters input (translated by user from keypress+keyboard state). Fill using AddInputCharacter() helper.
    float       NavInputs[PatoNavInput_COUNT]; // Gamepad inputs (keyboard keys will be auto-mapped and be written here by Pato::NewFrame, all values will be cleared back to zero in Pato::EndFrame)

    // Functions
    Pato_API void AddInputCharacter(ImWchar c);                        // Add new character into InputCharacters[]
    Pato_API void AddInputCharactersUTF8(const char* utf8_chars);      // Add new characters into InputCharacters[] from an UTF-8 string
    inline void    ClearInputCharacters() { InputCharacters[0] = 0; }   // Clear the text input buffer manually

    //------------------------------------------------------------------
    // Output - Retrieve after calling NewFrame()
    //------------------------------------------------------------------

    bool        WantCaptureMouse;           // When io.WantCaptureMouse is true, Pato will use the mouse inputs, do not dispatch them to your main game/application (in both cases, always pass on mouse inputs to Pato). (e.g. unclicked mouse is hovering over an Pato window, widget is active, mouse was clicked over an Pato window, etc.).
    bool        WantCaptureKeyboard;        // When io.WantCaptureKeyboard is true, Pato will use the keyboard inputs, do not dispatch them to your main game/application (in both cases, always pass keyboard inputs to Pato). (e.g. InputText active, or an Pato window is focused and navigation is enabled, etc.).
    bool        WantTextInput;              // Mobile/console: when io.WantTextInput is true, you may display an on-screen keyboard. This is set by Pato when it wants textual keyboard input to happen (e.g. when a InputText widget is active).
    bool        WantSetMousePos;            // MousePos has been altered, back-end should reposition mouse on next frame. Set only when PatoConfigFlags_NavEnableSetMousePos flag is enabled.
    bool        WantSaveIniSettings;        // When manual .ini load/save is active (io.IniFilename == NULL), this will be set to notify your application that you can call SaveIniSettingsToMemory() and save yourself. IMPORTANT: You need to clear io.WantSaveIniSettings yourself.
    bool        NavActive;                  // Directional navigation is currently allowed (will handle PatoKey_NavXXX events) = a window is focused and it doesn't use the PatoWindowFlags_NoNavInputs flag.
    bool        NavVisible;                 // Directional navigation is visible and allowed (will handle PatoKey_NavXXX events).
    float       Framerate;                  // Application framerate estimation, in frame per second. Solely for convenience. Rolling average estimation based on IO.DeltaTime over 120 frames
    int         MetricsRenderVertices;      // Vertices output during last call to Render()
    int         MetricsRenderIndices;       // Indices output during last call to Render() = number of triangles * 3
    int         MetricsRenderWindows;       // Number of visible windows
    int         MetricsActiveWindows;       // Number of active windows
    int         MetricsActiveAllocations;   // Number of active allocations, updated by MemAlloc/MemFree based on current context. May be off if you have multiple Pato contexts.
    ImVec2      MouseDelta;                 // Mouse delta. Note that this is zero if either current or previous position are invalid (-FLT_MAX,-FLT_MAX), so a disappearing/reappearing mouse won't have a huge delta.

    //------------------------------------------------------------------
    // [Internal] Pato will maintain those fields. Forward compatibility not guaranteed!
    //------------------------------------------------------------------

    ImVec2      MousePosPrev;               // Previous mouse position (note that MouseDelta is not necessary == MousePos-MousePosPrev, in case either position is invalid)
    ImVec2      MouseClickedPos[5];         // Position at time of clicking
    double      MouseClickedTime[5];        // Time of last click (used to figure out double-click)
    bool        MouseClicked[5];            // Mouse button went from !Down to Down
    bool        MouseDoubleClicked[5];      // Has mouse button been double-clicked?
    bool        MouseReleased[5];           // Mouse button went from Down to !Down
    bool        MouseDownOwned[5];          // Track if button was clicked inside a window. We don't request mouse capture from the application if click started outside Pato bounds.
    float       MouseDownDuration[5];       // Duration the mouse button has been down (0.0f == just clicked)
    float       MouseDownDurationPrev[5];   // Previous time the mouse button has been down
    ImVec2      MouseDragMaxDistanceAbs[5]; // Maximum distance, absolute, on each axis, of how much mouse has traveled from the clicking point
    float       MouseDragMaxDistanceSqr[5]; // Squared maximum distance of how much mouse has traveled from the clicking point
    float       KeysDownDuration[512];      // Duration the keyboard key has been down (0.0f == just pressed)
    float       KeysDownDurationPrev[512];  // Previous duration the key has been down
    float       NavInputsDownDuration[PatoNavInput_COUNT];
    float       NavInputsDownDurationPrev[PatoNavInput_COUNT];

    Pato_API   PatoIO();
};

//-----------------------------------------------------------------------------
// Misc data structures
//-----------------------------------------------------------------------------

// Shared state of InputText(), passed as an argument to your callback when a PatoInputTextFlags_Callback* flag is used.
// The callback function should return 0 by default.
// Callbacks (follow a flag name and see comments in PatoInputTextFlags_ declarations for more details)
// - PatoInputTextFlags_CallbackCompletion:  Callback on pressing TAB
// - PatoInputTextFlags_CallbackHistory:     Callback on pressing Up/Down arrows
// - PatoInputTextFlags_CallbackAlways:      Callback on each iteration
// - PatoInputTextFlags_CallbackCharFilter:  Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
// - PatoInputTextFlags_CallbackResize:      Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. 
struct PatoInputTextCallbackData
{
    PatoInputTextFlags EventFlag;      // One PatoInputTextFlags_Callback*    // Read-only
    PatoInputTextFlags Flags;          // What user passed to InputText()      // Read-only
    void*               UserData;       // What user passed to InputText()      // Read-only

    // Arguments for the different callback events
    // - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
    // - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
    ImWchar             EventChar;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
    PatoKey            EventKey;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
    char*               Buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
    int                 BufTextLen;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator storage. In C land: == strlen(some_text), in C++ land: string.length()
    int                 BufSize;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator storage. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
    bool                BufDirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
    int                 CursorPos;      //                                      // Read-write   // [Completion,History,Always]
    int                 SelectionLoopCallback; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
    int                 SelectionEnd;   //                                      // Read-write   // [Completion,History,Always]

    // Helper functions for text manipulation.
    // Use those function to benefit from the CallbackResize behaviors. Calling those function reset the selection.
    Pato_API PatoInputTextCallbackData();
    Pato_API void      DeleteChars(int pos, int bytes_count);
    Pato_API void      InsertChars(int pos, const char* text, const char* text_end = NULL);
    bool                HasSelection() const { return SelectionLoopCallback != SelectionEnd; }
};

// Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
struct PatoSizeCallbackData
{
    void*   UserData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
    ImVec2  Pos;            // Read-only.   Window position, for reference.
    ImVec2  CurrentSize;    // Read-only.   Current window size.
    ImVec2  DesiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
};

// Data payload for Drag and Drop operations: AcceptDragDropPayload(), GetDragDropPayload()
struct PatoPayload
{
    // Members
    void*           Data;               // Data (copied and owned by dear Pato)
    int             DataSize;           // Data size

    // [Internal]
    PatoID         SourceId;           // Source item id
    PatoID         SourceParentId;     // Source parent id (if available)
    int             DataFrameCount;     // Data timestamp
    char            DataType[32+1];     // Data type tag (short user-supplied string, 32 characters max)
    bool            Preview;            // Set when AcceptDragDropPayload() was called and mouse has been hovering the target item (nb: handle overlapping drag targets)
    bool            Delivery;           // Set when AcceptDragDropPayload() was called and mouse button is released over the target item.

    PatoPayload()  { Clear(); }
    void Clear()    { SourceId = SourceParentId = 0; Data = NULL; DataSize = 0; memset(DataType, 0, sizeof(DataType)); DataFrameCount = -1; Preview = Delivery = false; }
    bool IsDataType(const char* type) const { return DataFrameCount != -1 && strcmp(type, DataType) == 0; }
    bool IsPreview() const                  { return Preview; }
    bool IsDelivery() const                 { return Delivery; }
};

//-----------------------------------------------------------------------------
// Obsolete functions (Will be removed! Read 'API BREAKING CHANGES' section in Pato.cpp for details)
//-----------------------------------------------------------------------------

#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
namespace Pato
{
    // OBSOLETED in 1.66 (from Sep 2018)
    static inline void  SetScrollHere(float center_ratio=0.5f){ SetScrollHereY(center_ratio); }
    // OBSOLETED in 1.63 (from Aug 2018)
    static inline bool  IsItemDeactivatedAfterChange()        { return IsItemDeactivatedAfterEdit(); }
    // OBSOLETED in 1.61 (from Apr 2018)
    Pato_API bool      InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, PatoInputTextFlags extra_flags = 0); // Use the 'const char* format' version instead of 'decimal_precision'!
    Pato_API bool      InputFloat2(const char* label, float v[2], int decimal_precision, PatoInputTextFlags extra_flags = 0);
    Pato_API bool      InputFloat3(const char* label, float v[3], int decimal_precision, PatoInputTextFlags extra_flags = 0);
    Pato_API bool      InputFloat4(const char* label, float v[4], int decimal_precision, PatoInputTextFlags extra_flags = 0);
    // OBSOLETED in 1.60 (from Dec 2017)
    static inline bool  IsAnyWindowFocused()                  { return IsWindowFocused(PatoFocusedFlags_AnyWindow); }
    static inline bool  IsAnyWindowHovered()                  { return IsWindowHovered(PatoHoveredFlags_AnyWindow); }
    static inline ImVec2 CalcItemRectClosestPoint(const ImVec2& pos, bool on_edge = false, float outward = 0.f) { (void)on_edge; (void)outward; IM_ASSERT(0); return pos; }
    // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    static inline void  ShowTestWindow()                      { return ShowDemoWindow(); }
    static inline bool  IsRootWindowFocused()                 { return IsWindowFocused(PatoFocusedFlags_RootWindow); }
    static inline bool  IsRootWindowOrAnyChildFocused()       { return IsWindowFocused(PatoFocusedFlags_RootAndChildWindows); }
    static inline void  SetNextWindowContentWidth(float w)    { SetNextWindowContentSize(ImVec2(w, 0.0f)); }
    static inline float GetItemsLineHeightWithSpacing()       { return GetFrameHeightWithSpacing(); }
    // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
    Pato_API bool      Begin(const char* name, bool* p_open, const ImVec2& size_on_first_use, float bg_alpha_override = -1.0f, PatoWindowFlags flags = 0); // Use SetNextWindowSize(size, PatoCond_FirstUseEver) + SetNextWindowBgAlpha() instead.
    static inline bool  IsRootWindowOrAnyChildHovered()       { return IsWindowHovered(PatoHoveredFlags_RootAndChildWindows); }
    static inline void  AlignFirstTextHeightToWidgets()       { AlignTextToFramePadding(); }
    static inline void  SetNextWindowPosCenter(PatoCond c=0) { PatoIO& io = GetIO(); SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), c, ImVec2(0.5f, 0.5f)); }
    // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017)
    static inline bool  IsItemHoveredRect()                   { return IsItemHovered(PatoHoveredFlags_RectOnly); }
    static inline bool  IsPosHoveringAnyWindow(const ImVec2&) { IM_ASSERT(0); return false; } // This was misleading and partly broken. You probably want to use the Pato::GetIO().WantCaptureMouse flag instead.
    static inline bool  IsMouseHoveringAnyWindow()            { return IsWindowHovered(PatoHoveredFlags_AnyWindow); }
    static inline bool  IsMouseHoveringWindow()               { return IsWindowHovered(PatoHoveredFlags_AllowWhenBlockedByPopup | PatoHoveredFlags_AllowWhenBlockedByActiveItem); }
}
typedef PatoInputTextCallback      PatoTextEditCallback;      // OBSOLETE in 1.63 (from Aug 2018): made the names consistent
typedef PatoInputTextCallbackData  PatoTextEditCallbackData;
#endif

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Helper: Lightweight std::vector<> like class to avoid dragging dependencies (also: Windows implementation of STL with debug enabled is absurdly slow, so let's bypass it so our code runs fast in debug).
// *Important* Our implementation does NOT call C++ constructors/destructors. This is intentional, we do not require it but you have to be mindful of that. Do _not_ use this class as a std::vector replacement in your code!
template<typename T>
class ImVector
{
public:
    int                         Size;
    int                         Capacity;
    T*                          Data;

    typedef T                   value_type;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;

    inline ImVector()           { Size = Capacity = 0; Data = NULL; }
    inline ~ImVector()          { if (Data) Pato::MemFree(Data); }
    inline ImVector(const ImVector<T>& src)                     { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline ImVector<T>& operator=(const ImVector<T>& src)       { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(value_type)); return *this; }

    inline bool                 empty() const                   { return Size == 0; }
    inline int                  size() const                    { return Size; }
    inline int                  capacity() const                { return Capacity; }
    inline value_type&          operator[](int i)               { IM_ASSERT(i < Size); return Data[i]; }
    inline const value_type&    operator[](int i) const         { IM_ASSERT(i < Size); return Data[i]; }

    inline void                 clear()                         { if (Data) { Size = Capacity = 0; Pato::MemFree(Data); Data = NULL; } }
    inline iterator             begin()                         { return Data; }
    inline const_iterator       begin() const                   { return Data; }
    inline iterator             end()                           { return Data + Size; }
    inline const_iterator       end() const                     { return Data + Size; }
    inline value_type&          front()                         { IM_ASSERT(Size > 0); return Data[0]; }
    inline const value_type&    front() const                   { IM_ASSERT(Size > 0); return Data[0]; }
    inline value_type&          back()                          { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const value_type&    back() const                    { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void                 swap(ImVector<value_type>& rhs) { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; value_type* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const            { int new_capacity = Capacity ? (Capacity + Capacity/2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size)                    { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    inline void         resize(int new_size,const value_type& v){ if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
    inline void         reserve(int new_capacity)
    {
        if (new_capacity <= Capacity)
            return;
        value_type* new_data = (value_type*)Pato::MemAlloc((size_t)new_capacity * sizeof(value_type));
        if (Data)
        {
            memcpy(new_data, Data, (size_t)Size * sizeof(value_type));
            Pato::MemFree(Data);
        }
        Data = new_data;
        Capacity = new_capacity;
    }

    // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
    inline void         push_back(const value_type& v)                  { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
    inline void         pop_back()                                      { IM_ASSERT(Size > 0); Size--; }
    inline void         push_front(const value_type& v)                 { if (Size == 0) push_back(v); else insert(Data, v); }
    inline iterator     erase(const_iterator it)                        { IM_ASSERT(it >= Data && it < Data+Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(value_type)); Size--; return Data + off; }
    inline iterator     erase(const_iterator it, const_iterator it_last){ IM_ASSERT(it >= Data && it < Data+Size && it_last > it && it_last <= Data+Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - count) * sizeof(value_type)); Size -= (int)count; return Data + off; }
    inline iterator     erase_unsorted(const_iterator it)               { IM_ASSERT(it >= Data && it < Data+Size);  const ptrdiff_t off = it - Data; if (it < Data+Size-1) memcpy(Data + off, Data + Size - 1, sizeof(value_type)); Size--; return Data + off; }
    inline iterator     insert(const_iterator it, const value_type& v)  { IM_ASSERT(it >= Data && it <= Data+Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(value_type)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const value_type& v) const             { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline int          index_from_pointer(const_iterator it) const     { IM_ASSERT(it >= Data && it <= Data+Size); const ptrdiff_t off = it - Data; return (int)off; }
};

// Helper: IM_NEW(), IM_PLACEMENT_NEW(), IM_DELETE() macros to call MemAlloc + Placement New, Placement Delete + MemFree
// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a dummy parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
struct ImNewDummy {};
inline void* operator new(size_t, ImNewDummy, void* ptr) { return ptr; }
inline void  operator delete(void*, ImNewDummy, void*)   {} // This is only required so we can use the symetrical new()
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewDummy(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewDummy(), Pato::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void IM_DELETE(T* p)   { if (p) { p->~T(); Pato::MemFree(p); } }

// Helper: Execute a block of code at maximum once a frame. Convenient if you want to quickly create an UI within deep-nested code that runs multiple times every frame.
// Usage: static PatoOnceUponAFrame oaf; if (oaf) Pato::Text("This will be called only once per frame");
struct PatoOnceUponAFrame
{
    PatoOnceUponAFrame() { RefFrame = -1; }
    mutable int RefFrame;
    operator bool() const { int current_frame = Pato::GetFrameCount(); if (RefFrame == current_frame) return false; RefFrame = current_frame; return true; }
};

// Helper: Macro for PatoOnceUponAFrame. Attention: The macro expands into 2 statement so make sure you don't use it within e.g. an if() statement without curly braces.
#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS    // Will obsolete
#define Pato_ONCE_UPON_A_FRAME     static PatoOnceUponAFrame Pato_oaf; if (Pato_oaf)
#endif

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
struct PatoTextFilter
{
    Pato_API           PatoTextFilter(const char* default_filter = "");
    Pato_API bool      Draw(const char* label = "Filter (inc,-exc)", float width = 0.0f);    // Helper calling InputText+Build
    Pato_API bool      PassFilter(const char* text, const char* text_end = NULL) const;
    Pato_API void      Build();
    void                Clear()          { InputBuf[0] = 0; Build(); }
    bool                IsActive() const { return !Filters.empty(); }

    // [Internal]
    struct TextRange
    {
        const char* b;
        const char* e;

        TextRange() { b = e = NULL; }
        TextRange(const char* _b, const char* _e) { b = _b; e = _e; }
        const char*     begin() const   { return b; }
        const char*     end () const    { return e; }
        bool            empty() const   { return b == e; }
        Pato_API void  split(char separator, ImVector<TextRange>* out) const;
    };
    char                InputBuf[256];
    ImVector<TextRange> Filters;
    int                 CountGrep;
};

// Helper: Growable text buffer for logging/accumulating text
// (this could be called 'PatoTextBuilder' / 'PatoStringBuilder')
struct PatoTextBuffer
{
    ImVector<char>      Buf;
    static char         EmptyString[1];

    PatoTextBuffer()   { }
    inline char         operator[](int i)       { IM_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    const char*         begin() const           { return Buf.Data ? &Buf.front() : EmptyString; }
    const char*         end() const             { return Buf.Data ? &Buf.back() : EmptyString; }   // Buf is zero-terminated, so end() will point on the zero-terminator
    int                 size() const            { return Buf.Data ? Buf.Size - 1 : 0; }
    bool                empty()                 { return Buf.Size <= 1; }
    void                clear()                 { Buf.clear(); }
    void                reserve(int capacity)   { Buf.reserve(capacity); }
    const char*         c_str() const           { return Buf.Data ? Buf.Data : EmptyString; }
    Pato_API void      appendf(const char* fmt, ...) IM_FMTARGS(2);
    Pato_API void      appendfv(const char* fmt, va_list args) IM_FMTLIST(2);
};

// Helper: Key->Value storage
// Typically you don't have to worry about this since a storage is held within each Window.
// We use it to e.g. store collapse state for a tree (Int 0/1)
// This is optimized for efficient lookup (dichotomy into a contiguous buffer) and rare insertion (typically tied to user interactions aka max once a frame)
// You can use it as custom user storage for temporary values. Declare your own storage if, for example:
// - You want to manipulate the open/close state of a particular sub-tree in your interface (tree node uses Int 0/1 to store their state).
// - You want to store custom debug data easily without adding or editing structures in your code (probably not efficient, but convenient)
// Types are NOT stored, so it is up to you to make sure your Key don't collide with different types.
struct PatoStorage
{
    struct Pair
    {
        PatoID key;
        union { int val_i; float val_f; void* val_p; };
        Pair(PatoID _key, int _val_i)   { key = _key; val_i = _val_i; }
        Pair(PatoID _key, float _val_f) { key = _key; val_f = _val_f; }
        Pair(PatoID _key, void* _val_p) { key = _key; val_p = _val_p; }
    };
    ImVector<Pair>      Data;

    // - Get***() functions find pair, never add/allocate. Pairs are sorted so a query is O(log N)
    // - Set***() functions find pair, insertion on demand if missing.
    // - Sorted insertion is costly, paid once. A typical frame shouldn't need to insert any new pair.
    void                Clear() { Data.clear(); }
    Pato_API int       GetInt(PatoID key, int default_val = 0) const;
    Pato_API void      SetInt(PatoID key, int val);
    Pato_API bool      GetBool(PatoID key, bool default_val = false) const;
    Pato_API void      SetBool(PatoID key, bool val);
    Pato_API float     GetFloat(PatoID key, float default_val = 0.0f) const;
    Pato_API void      SetFloat(PatoID key, float val);
    Pato_API void*     GetVoidPtr(PatoID key) const; // default_val is NULL
    Pato_API void      SetVoidPtr(PatoID key, void* val);

    // - Get***Ref() functions finds pair, insert on demand if missing, return pointer. Useful if you intend to do Get+Set.
    // - References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
    // - A typical use case where this is convenient for quick hacking (e.g. add storage during a live Edit&Continue session if you can't modify existing struct)
    //      float* pvar = Pato::GetFloatRef(key); Pato::SliderFloat("var", pvar, 0, 100.0f); some_var += *pvar;
    Pato_API int*      GetIntRef(PatoID key, int default_val = 0);
    Pato_API bool*     GetBoolRef(PatoID key, bool default_val = false);
    Pato_API float*    GetFloatRef(PatoID key, float default_val = 0.0f);
    Pato_API void**    GetVoidPtrRef(PatoID key, void* default_val = NULL);

    // Use on your own storage if you know only integer are being stored (open/close all tree nodes)
    Pato_API void      SetAllInt(int val);

    // For quicker full rebuild of a storage (instead of an incremental one), you may add all your contents and then sort once.
    Pato_API void      BuildSortByKey();
};

// Helper: Manually clip large list of items.
// If you are submitting lots of evenly spaced items and you have a random access to the list, you can perform coarse clipping based on visibility to save yourself from processing those items at all.
// The clipper calculates the range of visible items and advance the cursor to compensate for the non-visible items we have skipped.
// Pato already clip items based on their bounds but it needs to measure text size to do so. Coarse clipping before submission makes this cost and your own data fetching/submission cost null.
// Usage:
//     PatoListClipper clipper(1000);  // we have 1000 elements, evenly spaced.
//     while (clipper.Step())
//         for (int i = clipper.DisplayLoopCallback; i < clipper.DisplayEnd; i++)
//             Pato::Text("line number %d", i);
// - Step 0: the clipper let you process the first element, regardless of it being visible or not, so we can measure the element height (step skipped if we passed a known height as second arg to constructor).
// - Step 1: the clipper infer height from first element, calculate the actual range of elements to display, and position the cursor before the first element.
// - (Step 2: dummy step only required if an explicit items_height was passed to constructor or Begin() and user call Step(). Does nothing and switch to Step 3.)
// - Step 3: the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd), advance the cursor to the end of the list and then returns 'false' to end the loop.
struct PatoListClipper
{
    float   LoopCallbackPosY;
    float   ItemsHeight;
    int     ItemsCount, StepNo, DisplayLoopCallback, DisplayEnd;

    // items_count:  Use -1 to ignore (you can call Begin later). Use INT_MAX if you don't know how many items you have (in which case the cursor won't be advanced in the final step).
    // items_height: Use -1.0f to be calculated automatically on first step. Otherwise pass in the distance between your items, typically GetTextLineHeightWithSpacing() or GetFrameHeightWithSpacing().
    // If you don't specify an items_height, you NEED to call Step(). If you specify items_height you may call the old Begin()/End() api directly, but prefer calling Step().
    PatoListClipper(int items_count = -1, float items_height = -1.0f)  { Begin(items_count, items_height); } // NB: Begin() initialize every fields (as we allow user to call Begin/End multiple times on a same instance if they want).
    ~PatoListClipper()                                                 { IM_ASSERT(ItemsCount == -1); }      // Assert if user forgot to call End() or Step() until false.

    Pato_API bool Step();                                              // Call until it returns false. The DisplayLoopCallback/DisplayEnd fields will be set and you can process/draw those items.
    Pato_API void Begin(int items_count, float items_height = -1.0f);  // Automatically called by constructor if you passed 'items_count' or by Step() in Step 1.
    Pato_API void End();                                               // Automatically called on the last call of Step() that returns false.
};

// Helpers macros to generate 32-bits encoded colors
#ifdef Pato_USE_BGRA_PACKED_COLOR
#define IM_COL32_R_SHIFT    16
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    0
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#else
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#endif
#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000

// Helper: ImColor() implicity converts colors to either ImU32 (packed 4x1 byte) or ImVec4 (4x1 float)
// Prefer using IM_COL32() macros if you want a guaranteed compile-time ImU32 for usage with ImDrawList API.
// **Avoid storing ImColor! Store either u32 of ImVec4. This is not a full-featured color class. MAY OBSOLETE.
// **None of the Pato API are using ImColor directly but you can use it as a convenience to pass colors in either ImU32 or ImVec4 formats. Explicitly cast to ImU32 or ImVec4 if needed.
struct ImColor
{
    ImVec4              Value;

    ImColor()                                                       { Value.x = Value.y = Value.z = Value.w = 0.0f; }
    ImColor(int r, int g, int b, int a = 255)                       { float sc = 1.0f/255.0f; Value.x = (float)r * sc; Value.y = (float)g * sc; Value.z = (float)b * sc; Value.w = (float)a * sc; }
    ImColor(ImU32 rgba)                                             { float sc = 1.0f/255.0f; Value.x = (float)((rgba>>IM_COL32_R_SHIFT)&0xFF) * sc; Value.y = (float)((rgba>>IM_COL32_G_SHIFT)&0xFF) * sc; Value.z = (float)((rgba>>IM_COL32_B_SHIFT)&0xFF) * sc; Value.w = (float)((rgba>>IM_COL32_A_SHIFT)&0xFF) * sc; }
    ImColor(float r, float g, float b, float a = 1.0f)              { Value.x = r; Value.y = g; Value.z = b; Value.w = a; }
    ImColor(const ImVec4& col)                                      { Value = col; }
    inline operator ImU32() const                                   { return Pato::ColorConvertFloat4ToU32(Value); }
    inline operator ImVec4() const                                  { return Value; }

    // FIXME-OBSOLETE: May need to obsolete/cleanup those helpers.
    inline void    SetHSV(float h, float s, float v, float a = 1.0f){ Pato::ColorConvertHSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }
    static ImColor HSV(float h, float s, float v, float a = 1.0f)   { float r,g,b; Pato::ColorConvertHSVtoRGB(h, s, v, r, g, b); return ImColor(r,g,b,a); }
};

//-----------------------------------------------------------------------------
// Draw List API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

// Draw callbacks for advanced uses.
// NB: You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering,
// you can poke into the draw list for that! Draw callback may be useful for example to: A) Change your GPU render state, 
// B) render a complex 3D scene inside a UI element without an intermediate texture/render target, etc.
// The expected behavior from your rendering function is 'if (cmd.UserCallback != NULL) { cmd.UserCallback(parent_list, cmd); } else { RenderTriangles() }'
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);

// Typically, 1 command = 1 GPU draw call (unless command is a callback)
struct ImDrawCmd
{
    unsigned int    ElemCount;              // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
    ImVec4          ClipRect;               // Clipping rectangle (x1, y1, x2, y2). Subtract ImDrawData->DisplayPos to get clipping rectangle in "viewport" coordinates
    ImTextureID     TextureId;              // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
    ImDrawCallback  UserCallback;           // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
    void*           UserCallbackData;       // The draw callback code can access this.

    ImDrawCmd() { ElemCount = 0; ClipRect.x = ClipRect.y = ClipRect.z = ClipRect.w = 0.0f; TextureId = (ImTextureID)NULL; UserCallback = NULL; UserCallbackData = NULL; }
};

// Vertex index (override with '#define ImDrawIdx unsigned int' in imconfig.h)
#ifndef ImDrawIdx
typedef unsigned short ImDrawIdx;
#endif

// Vertex layout
#ifndef Pato_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
struct ImDrawVert
{
    ImVec2  pos;
    ImVec2  uv;
    ImU32   col;
};
#else
// You can override the vertex format layout by defining Pato_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect ImVec2 pos (8 bytes), ImVec2 uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef)
// NOTE: Pato DOESN'T CLEAR THE STRUCTURE AND DOESN'T CALL A CONSTRUCTOR SO ANY CUSTOM FIELD WILL BE UNINITIALIZED. IF YOU ADD EXTRA FIELDS (SUCH AS A 'Z' COORDINATES) YOU WILL NEED TO CLEAR THEM DURING RENDER OR TO IGNORE THEM.
Pato_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

// Draw channels are used by the Columns API to "split" the render list into different channels while building, so items of each column can be batched together.
// You can also use them to simulate drawing layers and submit primitives in a different order than how they will be rendered.
struct ImDrawChannel
{
    ImVector<ImDrawCmd>     CmdBuffer;
    ImVector<ImDrawIdx>     IdxBuffer;
};

enum ImDrawCornerFlags_
{
    ImDrawCornerFlags_TopLeft   = 1 << 0, // 0x1
    ImDrawCornerFlags_TopRight  = 1 << 1, // 0x2
    ImDrawCornerFlags_BotLeft   = 1 << 2, // 0x4
    ImDrawCornerFlags_BotRight  = 1 << 3, // 0x8
    ImDrawCornerFlags_Top       = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,   // 0x3
    ImDrawCornerFlags_Bot       = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,   // 0xC
    ImDrawCornerFlags_Left      = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,    // 0x5
    ImDrawCornerFlags_Right     = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight,  // 0xA
    ImDrawCornerFlags_All       = 0xF     // In your function calls you may use ~0 (= all bits sets) instead of ImDrawCornerFlags_All, as a convenience
};

enum ImDrawListFlags_
{
    ImDrawListFlags_None             = 0,
    ImDrawListFlags_AntiAliasedLines = 1 << 0,  // Lines are anti-aliased (*2 the number of triangles for 1.0f wide line, otherwise *3 the number of triangles)
    ImDrawListFlags_AntiAliasedFill  = 1 << 1   // Filled shapes have anti-aliased edges (*2 the number of vertices)
};

// Draw command list
// This is the low-level list of polygons that Pato functions are filling. At the end of the frame, all command lists are passed to your PatoIO::RenderDrawListFn function for rendering.
// Each Pato window contains its own ImDrawList. You can use Pato::GetWindowDrawList() to access the current window draw list and draw custom primitives.
// You can interleave normal Pato:: calls and adding primitives to the current draw list.
// All positions are generally in pixel coordinates (top-left at (0,0), bottom-right at io.DisplaySize), but you are totally free to apply whatever transformation matrix to want to the data (if you apply such transformation you'll want to apply it to ClipRect as well)
// Important: Primitives are always added to the list and not culled (culling is done at higher-level by Pato:: functions), if you use this API a lot consider coarse culling your drawn objects.
struct ImDrawList
{
    // This is what you have to render
    ImVector<ImDrawCmd>     CmdBuffer;          // Draw commands. Typically 1 command = 1 GPU draw call, unless the command is a callback.
    ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
    ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
    ImDrawListFlags         Flags;              // Flags, you may poke into these to adjust anti-aliasing settings per-primitive.

    // [Internal, used while building lists]
    const ImDrawListSharedData* _Data;          // Pointer to shared draw data (you can use Pato::GetDrawListSharedData() to get the one from current Pato context)
    const char*             _OwnerName;         // Pointer to owner window's name for debugging
    unsigned int            _VtxCurrentIdx;     // [Internal] == VtxBuffer.Size
    ImDrawVert*             _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImDrawIdx*              _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImVector<ImVec4>        _ClipRectStack;     // [Internal]
    ImVector<ImTextureID>   _TextureIdStack;    // [Internal]
    ImVector<ImVec2>        _Path;              // [Internal] current path building
    int                     _ChannelsCurrent;   // [Internal] current channel number (0)
    int                     _ChannelsCount;     // [Internal] number of active channels (1+)
    ImVector<ImDrawChannel> _Channels;          // [Internal] draw channels for columns API (not resized down so _ChannelsCount may be smaller than _Channels.Size)

    // If you want to create ImDrawList instances, pass them Pato::GetDrawListSharedData() or create and use your own ImDrawListSharedData (so you can use ImDrawList without Pato)
    ImDrawList(const ImDrawListSharedData* shared_data) { _Data = shared_data; _OwnerName = NULL; Clear(); }
    ~ImDrawList() { ClearFreeMemory(); }
    Pato_API void  PushClipRect(ImVec2 clip_rect_min, ImVec2 clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level Pato::PushClipRect() to affect logic (hit-testing and widget culling)
    Pato_API void  PushClipRectFullScreen();
    Pato_API void  PopClipRect();
    Pato_API void  PushTextureID(ImTextureID texture_id);
    Pato_API void  PopTextureID();
    inline ImVec2   GetClipRectMin() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.x, cr.y); }
    inline ImVec2   GetClipRectMax() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.z, cr.w); }

    // Primitives
    Pato_API void  AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
    Pato_API void  AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.0f);   // a: upper-left, b: lower-right, rounding_corners_flags: 4-bits corresponding to which corner to round
    Pato_API void  AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);                     // a: upper-left, b: lower-right
    Pato_API void  AddRectFilledMultiColor(const ImVec2& a, const ImVec2& b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    Pato_API void  AddTriangleFilledMultiColor(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col_a, ImU32 col_b, ImU32 col_c);
    Pato_API void  AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness = 1.0f);
    Pato_API void  AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col);
    Pato_API void  AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness = 1.0f);
    Pato_API void  AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col);
    Pato_API void  AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12, float thickness = 1.0f);
    Pato_API void  AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12);
    Pato_API void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    Pato_API void  AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
    Pato_API void  AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0,0), const ImVec2& uv_b = ImVec2(1,1), ImU32 col = 0xFFFFFFFF);
    Pato_API void  AddImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a = ImVec2(0,0), const ImVec2& uv_b = ImVec2(1,0), const ImVec2& uv_c = ImVec2(1,1), const ImVec2& uv_d = ImVec2(0,1), ImU32 col = 0xFFFFFFFF);
    Pato_API void  AddImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners = ImDrawCornerFlags_All);
    Pato_API void  AddPolyline(const ImVec2* points, const int num_points, ImU32 col, bool closed, float thickness);
    Pato_API void  AddConvexPolyFilled(const ImVec2* points, const int num_points, ImU32 col); // Note: Anti-aliased filling requires points to be in clockwise order.
    Pato_API void  AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments = 0);

    // Stateful path API, add points then finish with PathFillConvex() or PathStroke()
    inline    void  PathClear()                                                 { _Path.resize(0); }
    inline    void  PathLineTo(const ImVec2& pos)                               { _Path.push_back(pos); }
    inline    void  PathLineToMergeDuplicate(const ImVec2& pos)                 { if (_Path.Size == 0 || memcmp(&_Path[_Path.Size-1], &pos, 8) != 0) _Path.push_back(pos); }
    inline    void  PathFillConvex(ImU32 col)                                   { AddConvexPolyFilled(_Path.Data, _Path.Size, col); PathClear(); }  // Note: Anti-aliased filling requires points to be in clockwise order.
    inline    void  PathStroke(ImU32 col, bool closed, float thickness = 1.0f)  { AddPolyline(_Path.Data, _Path.Size, col, closed, thickness); PathClear(); }
    Pato_API void  PathArcTo(const ImVec2& centre, float radius, float a_min, float a_max, int num_segments = 10);
    Pato_API void  PathArcToFast(const ImVec2& centre, float radius, int a_min_of_12, int a_max_of_12);                                            // Use precomputed angles for a 12 steps circle
    Pato_API void  PathBezierCurveTo(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, int num_segments = 0);
    Pato_API void  PathRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);

    // Channels
    // - Use to simulate layers. By switching channels to can render out-of-order (e.g. submit foreground primitives before background primitives)
    // - Use to minimize draw calls (e.g. if going back-and-forth between multiple non-overlapping clipping rectangles, prefer to append into separate channels then merge at the end)
    Pato_API void  ChannelsSplit(int channels_count);
    Pato_API void  ChannelsMerge();
    Pato_API void  ChannelsSetCurrent(int channel_index);

    // Advanced
    Pato_API void  AddCallback(ImDrawCallback callback, void* callback_data);  // Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles.
    Pato_API void  AddDrawCmd();                                               // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible
    Pato_API ImDrawList* CloneOutput() const;                                  // Create a clone of the CmdBuffer/IdxBuffer/VtxBuffer.

    // Internal helpers
    // NB: all primitives needs to be reserved via PrimReserve() beforehand!
    Pato_API void  Clear();
    Pato_API void  ClearFreeMemory();
    Pato_API void  PrimReserve(int idx_count, int vtx_count);
    Pato_API void  PrimRect(const ImVec2& a, const ImVec2& b, ImU32 col);      // Axis aligned rectangle (composed of two triangles)
    Pato_API void  PrimRectUV(const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col);
    Pato_API void  PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col);
    inline    void  PrimWriteVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col){ _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
    inline    void  PrimWriteIdx(ImDrawIdx idx)                                 { *_IdxWritePtr = idx; _IdxWritePtr++; }
    inline    void  PrimVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col)     { PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); }
    Pato_API void  UpdateClipRect();
    Pato_API void  UpdateTextureID();
};

// All draw data to render an Pato frame
// (NB: the style and the naming convention here is a little inconsistent but we preserve them for backward compatibility purpose)
struct ImDrawData
{
    bool            Valid;                  // Only valid after Render() is called and before the next NewFrame() is called.
    ImDrawList**    CmdLists;               // Array of ImDrawList* to render. The ImDrawList are owned by PatoContext and only pointed to from here.
    int             CmdListsCount;          // Number of ImDrawList* to render
    int             TotalIdxCount;          // For convenience, sum of all ImDrawList's IdxBuffer.Size
    int             TotalVtxCount;          // For convenience, sum of all ImDrawList's VtxBuffer.Size
    ImVec2          DisplayPos;             // Upper-left position of the viewport to render (== upper-left of the orthogonal projection matrix to use)
    ImVec2          DisplaySize;            // Size of the viewport to render (== io.DisplaySize for the main viewport) (DisplayPos + DisplaySize == lower-right of the orthogonal projection matrix to use)

    // Functions
    ImDrawData()    { Valid = false; Clear(); }
    ~ImDrawData()   { Clear(); }
    void Clear()    { Valid = false; CmdLists = NULL; CmdListsCount = TotalVtxCount = TotalIdxCount = 0; DisplayPos = DisplaySize = ImVec2(0.f, 0.f); } // The ImDrawList are owned by PatoContext!
    Pato_API void  DeIndexAllBuffers();                // Helper to convert all buffers from indexed to non-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
    Pato_API void  ScaleClipRects(const ImVec2& sc);   // Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than Pato expects, or if there is a difference between your window resolution and framebuffer resolution.
};

//-----------------------------------------------------------------------------
// Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFont)
//-----------------------------------------------------------------------------

struct ImFontConfig
{
    void*           FontData;               //          // TTF/OTF data
    int             FontDataSize;           //          // TTF/OTF data size
    bool            FontDataOwnedByAtlas;   // true     // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
    int             FontNo;                 // 0        // Index of font within TTF/OTF file
    float           SizePixels;             //          // Size in pixels for rasterizer (more or less maps to the resulting font height).
    int             OversampleH;            // 3        // Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
    int             OversampleV;            // 1        // Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
    bool            PixelSnapH;             // false    // Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
    ImVec2          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
    ImVec2          GlyphOffset;            // 0, 0     // Offset all glyphs from this font input.
    const ImWchar*  GlyphRanges;            // NULL     // Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list). THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE.
    float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
    float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
    bool            MergeMode;              // false    // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
    unsigned int    RasterizerFlags;        // 0x00     // Settings for custom font rasterizer (e.g. PatoFreeType). Leave as zero if you aren't using one.
    float           RasterizerMultiply;     // 1.0f     // Brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable.

    // [Internal]
    char            Name[40];               // Name (strictly to ease debugging)
    ImFont*         DstFont;

    Pato_API ImFontConfig();
};

struct ImFontGlyph
{
    ImWchar         Codepoint;          // 0x0000..0xFFFF
    float           AdvanceX;           // Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
    float           X0, Y0, X1, Y1;     // Glyph corners
    float           U0, V0, U1, V1;     // Texture coordinates
};

enum ImFontAtlasFlags_
{
    ImFontAtlasFlags_None               = 0,
    ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
    ImFontAtlasFlags_NoMouseCursors     = 1 << 1    // Don't build software mouse cursors into the atlas
};

// Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
//  - One or more fonts.
//  - Custom graphics data needed to render the shapes needed by Dear Pato.
//  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
// It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
//  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
//  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
//  - Upload the pixels data into a texture within your graphics system (see Pato_impl_xxxx.cpp examples)
//  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API. 
//    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
// Common pitfalls:
// - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the 
//   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
// - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
//   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed, 
// - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
// - This is an old API and it is currently awkward for those and and various other reasons! We will address them in the future!
struct ImFontAtlas
{
    Pato_API ImFontAtlas();
    Pato_API ~ImFontAtlas();
    Pato_API ImFont*           AddFont(const ImFontConfig* font_cfg);
    Pato_API ImFont*           AddFontDefault(const ImFontConfig* font_cfg = NULL);
    Pato_API ImFont*           AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
    Pato_API ImFont*           AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    Pato_API ImFont*           AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
    Pato_API ImFont*           AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.
    Pato_API void              ClearInputData();           // Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
    Pato_API void              ClearTexData();             // Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
    Pato_API void              ClearFonts();               // Clear output font data (glyphs storage, UV coordinates).
    Pato_API void              Clear();                    // Clear all input and output.

    // Build atlas, retrieve pixel data.
    // User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
    // The pitch is always = Width * BytesPerPixels (1 or 4)
    // Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into 
    // the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
    Pato_API bool              Build();                    // Build pixels data. This is called automatically for you by the GetTexData*** functions.
    Pato_API void              GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 1 byte per-pixel
    Pato_API void              GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 4 bytes-per-pixel
    bool                        IsBuilt()                   { return Fonts.Size > 0 && (TexPixelsAlpha8 != NULL || TexPixelsRGBA32 != NULL); }
    void                        SetTexID(ImTextureID id)    { TexID = id; }

    //-------------------------------------------
    // Glyph Ranges
    //-------------------------------------------

    // Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
    // NB: Make sure that your string are UTF-8 and NOT in your local code page. In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax. See FAQ for details.
    // NB: Consider using GlyphRangesBuilder to build glyph ranges from textual data.
    Pato_API const ImWchar*    GetGlyphRangesDefault();                // Basic Latin, Extended Latin
    Pato_API const ImWchar*    GetGlyphRangesKorean();                 // Default + Korean characters
    Pato_API const ImWchar*    GetGlyphRangesJapanese();               // Default + Hiragana, Katakana, Half-Width, Selection of 1946 Ideographs
    Pato_API const ImWchar*    GetGlyphRangesChineseFull();            // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
    Pato_API const ImWchar*    GetGlyphRangesChineseSimplifiedCommon();// Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
    Pato_API const ImWchar*    GetGlyphRangesCyrillic();               // Default + about 400 Cyrillic characters
    Pato_API const ImWchar*    GetGlyphRangesThai();                   // Default + Thai characters

    // Helpers to build glyph ranges from text data. Feed your application strings/characters to it then call BuildRanges().
    struct GlyphRangesBuilder
    {
        ImVector<unsigned char> UsedChars;  // Store 1-bit per Unicode code point (0=unused, 1=used)
        GlyphRangesBuilder()                { UsedChars.resize(0x10000 / 8); memset(UsedChars.Data, 0, 0x10000 / 8); }
        bool           GetBit(int n) const  { return (UsedChars[n >> 3] & (1 << (n & 7))) != 0; }
        void           SetBit(int n)        { UsedChars[n >> 3] |= 1 << (n & 7); }  // Set bit 'c' in the array
        void           AddChar(ImWchar c)   { SetBit(c); }                          // Add character
        Pato_API void AddText(const char* text, const char* text_end = NULL);      // Add string (each character of the UTF-8 string are added)
        Pato_API void AddRanges(const ImWchar* ranges);                            // Add ranges, e.g. builder.AddRanges(ImFontAtlas::GetGlyphRangesDefault()) to force add all of ASCII/Latin+Ext
        Pato_API void BuildRanges(ImVector<ImWchar>* out_ranges);                  // Output new ranges
    };

    //-------------------------------------------
    // Custom Rectangles/Glyphs API
    //-------------------------------------------

    // You can request arbitrary rectangles to be packed into the atlas, for your own purposes. After calling Build(), you can query the rectangle position and render your pixels.
    // You can also request your rectangles to be mapped as font glyph (given a font + Unicode point), so you can render e.g. custom colorful icons and use them as regular glyphs.
    struct CustomRect
    {
        unsigned int    ID;             // Input    // User ID. Use <0x10000 to map into a font glyph, >=0x10000 for other/internal/custom texture data.
        unsigned short  Width, Height;  // Input    // Desired rectangle dimension
        unsigned short  X, Y;           // Output   // Packed position in Atlas
        float           GlyphAdvanceX;  // Input    // For custom font glyphs only (ID<0x10000): glyph xadvance
        ImVec2          GlyphOffset;    // Input    // For custom font glyphs only (ID<0x10000): glyph display offset
        ImFont*         Font;           // Input    // For custom font glyphs only (ID<0x10000): target font
        CustomRect()            { ID = 0xFFFFFFFF; Width = Height = 0; X = Y = 0xFFFF; GlyphAdvanceX = 0.0f; GlyphOffset = ImVec2(0,0); Font = NULL; }
        bool IsPacked() const   { return X != 0xFFFF; }
    };

    Pato_API int       AddCustomRectRegular(unsigned int id, int width, int height);                                                                   // Id needs to be >= 0x10000. Id >= 0x80000000 are reserved for Pato and ImDrawList
    Pato_API int       AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset = ImVec2(0,0));   // Id needs to be < 0x10000 to register a rectangle to map into a specific font.
    const CustomRect*   GetCustomRectByIndex(int index) const { if (index < 0) return NULL; return &CustomRects[index]; }

    // [Internal]
    Pato_API void      CalcCustomRectUV(const CustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max);
    Pato_API bool      GetMouseCursorTexData(PatoMouseCursor cursor, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2]);

    //-------------------------------------------
    // Members
    //-------------------------------------------

    bool                        Locked;             // Marked as Locked by Pato::NewFrame() so attempt to modify the atlas will assert.
    ImFontAtlasFlags            Flags;              // Build flags (see ImFontAtlasFlags_)
    ImTextureID                 TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
    int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
    int                         TexGlyphPadding;    // Padding between glyphs within texture in pixels. Defaults to 1.

    // [Internal]
    // NB: Access texture data via GetTexData*() calls! Which will setup a default font for you.
    unsigned char*              TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
    unsigned int*               TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
    int                         TexWidth;           // Texture width calculated during Build().
    int                         TexHeight;          // Texture height calculated during Build().
    ImVec2                      TexUvScale;         // = (1.0f/TexWidth, 1.0f/TexHeight)
    ImVec2                      TexUvWhitePixel;    // Texture coordinates to a white pixel
    ImVector<ImFont*>           Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling Pato::NewFrame(), use Pato::PushFont()/PopFont() to change the current font.
    ImVector<CustomRect>        CustomRects;        // Rectangles for packing custom texture data into the atlas.
    ImVector<ImFontConfig>      ConfigData;         // Internal data
    int                         CustomRectIds[1];   // Identifiers of custom texture rectangle used by ImFontAtlas/ImDrawList
};

// Font runtime data and rendering
// ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
struct ImFont
{
    // Members: Hot ~62/78 bytes
    float                       FontSize;           // <user set>   // Height of characters, set during loading (don't change after loading)
    float                       Scale;              // = 1.f        // Base font scale, multiplied by the per-window font scale which you can adjust with SetFontScale()
    ImVec2                      DisplayOffset;      // = (0.f,0.f)  // Offset font rendering by xx pixels
    ImVector<ImFontGlyph>       Glyphs;             //              // All glyphs.
    ImVector<float>             IndexAdvanceX;      //              // Sparse. Glyphs->AdvanceX in a directly indexable way (more cache-friendly, for CalcTextSize functions which are often bottleneck in large UI).
    ImVector<ImWchar>           IndexLookup;        //              // Sparse. Index glyphs by Unicode code-point.
    const ImFontGlyph*          FallbackGlyph;      // == FindGlyph(FontFallbackChar)
    float                       FallbackAdvanceX;   // == FallbackGlyph->AdvanceX
    ImWchar                     FallbackChar;       // = '?'        // Replacement glyph if one isn't found. Only set via SetFallbackChar()

    // Members: Cold ~18/26 bytes
    short                       ConfigDataCount;    // ~ 1          // Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
    ImFontConfig*               ConfigData;         //              // Pointer within ContainerAtlas->ConfigData
    ImFontAtlas*                ContainerAtlas;     //              // What we has been loaded into
    float                       Ascent, Descent;    //              // Ascent: distance from top to bottom of e.g. 'A' [0..FontSize]
    bool                        DirtyLookupTables;
    int                         MetricsTotalSurface;//              // Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)

    // Methods
    Pato_API ImFont();
    Pato_API ~ImFont();
    Pato_API void              ClearOutputData();
    Pato_API void              BuildLookupTable();
    Pato_API const ImFontGlyph*FindGlyph(ImWchar c) const;
    Pato_API const ImFontGlyph*FindGlyphNoFallback(ImWchar c) const;
    Pato_API void              SetFallbackChar(ImWchar c);
    float                       GetCharAdvance(ImWchar c) const     { return ((int)c < IndexAdvanceX.Size) ? IndexAdvanceX[(int)c] : FallbackAdvanceX; }
    bool                        IsLoaded() const                    { return ContainerAtlas != NULL; }
    const char*                 GetDebugName() const                { return ConfigData ? ConfigData->Name : "<unknown>"; }

    // 'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
    // 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
    Pato_API ImVec2            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const; // utf8
    Pato_API const char*       CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;
    Pato_API void              RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const;
    Pato_API void              RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width = 0.0f, bool cpu_fine_clip = false) const;

    // [Internal]
    Pato_API void              GrowIndex(int new_size);
    Pato_API void              AddGlyph(ImWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
    Pato_API void              AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.

#ifndef Pato_DISABLE_OBSOLETE_FUNCTIONS
    typedef ImFontGlyph Glyph; // OBSOLETE 1.52+
#endif
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic pop
#endif

// Include Pato_user.h at the end of Pato.h (convenient for user to only explicitly include vanilla Pato.h)
#ifdef Pato_INCLUDE_Pato_USER_H
#include "Pato_user.h"
#endif
