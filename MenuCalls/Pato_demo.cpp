// dear Pato, v1.67 WIP
// (demo code)

// Message to the person tempted to delete this file when integrating Dear Pato into their code base:
// Do NOT remove this file from your project! Think again! It is the most useful reference code that you and other coders
// will want to refer to and call. Have the Pato::ShowDemoWindow() function wired in an always-available debug menu of 
// your game/app! Removing this file from your project is hindering access to documentation for everyone in your team, 
// likely leading you to poorer usage of the library.
// Everything in this file will be stripped out by the linker if you don't call Pato::ShowDemoWindow().
// If you want to link core Dear Pato in your shipped builds but want an easy guarantee that the demo will not be linked, 
// you can setup your imconfig.h with #define Pato_DISABLE_DEMO_WINDOWS and those functions will be empty.
// In other situation, whenever you have Dear Pato available you probably want this to be available for reference.
// Thank you,
// -Your beloved friend, Pato_demo.cpp (that you won't delete)

// Message to beginner C/C++ programmers about the meaning of the 'static' keyword: 
// In this demo code, we frequently we use 'static' variables inside functions. A static variable persist across calls, so it is 
// essentially like a global variable but declared inside the scope of the function. We do this as a way to gather code and data 
// in the same place, to make the demo source code faster to read, faster to write, and smaller in size.
// It also happens to be a convenient way of storing simple UI related information as long as your function doesn't need to be reentrant
// or used in threads. This might be a pattern you will want to use in your code, but most of the real data you would be editing is 
// likely going to be stored outside your functions.

/*

Index of this file:

// [SECTION] Forward Declarations, Helpers
// [SECTION] Demo Window / ShowDemoWindow()
// [SECTION] About Window / ShowAboutWindow()
// [SECTION] Style Editor / ShowStyleEditor()
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Pato.h"
#include <ctype.h>          // toupper, isprint
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define vsnprintf _vsnprintf
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"             // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"    // warning : 'xx' is deprecated: The POSIX name for this item.. // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"   // warning : cast to 'void *' from smaller integer type 'int'
#pragma clang diagnostic ignored "-Wformat-security"            // warning : warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"      // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. Pato coding style welcomes static/globals.
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"          // warning : macro name is a reserved identifier                //
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"          // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"              // warning : format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"             // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"                   // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#if (__GNUC__ >= 6)
#pragma GCC diagnostic ignored "-Wmisleading-indentation"       // warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif
#endif

// Play it nice with Windows users. Notepad in 2017 still doesn't display text data with Unix-style \n.
#ifdef _WIN32
#define IM_NEWLINE "\r\n"
#else
#define IM_NEWLINE "\n"
#endif

#define IM_MAX(_A,_B)       (((_A) >= (_B)) ? (_A) : (_B))

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(Pato_DISABLE_OBSOLETE_FUNCTIONS) && defined(Pato_DISABLE_TEST_WINDOWS) && !defined(Pato_DISABLE_DEMO_WINDOWS)   // Obsolete name since 1.53, TEST->DEMO
#define Pato_DISABLE_DEMO_WINDOWS
#endif

#if !defined(Pato_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppDocuments(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleMenuFile();

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void ShowHelpMarker(const char* desc)
{
    Pato::TextDisabled("(?)");
    if (Pato::IsItemHovered())
    {
        Pato::BeginTooltip();
        Pato::PushTextWrapPos(Pato::GetFontSize() * 35.0f);
        Pato::TextUnformatted(desc);
        Pato::PopTextWrapPos();
        Pato::EndTooltip();
    }
}

// Helper to display basic user controls.
void Pato::ShowUserGuide()
{
    Pato::BulletText("Double-click on title bar to collapse window.");
    Pato::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
    Pato::BulletText("Click and drag on any empty space to move window.");
    Pato::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    Pato::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    if (Pato::GetIO().FontAllowUserScaling)
        Pato::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    Pato::BulletText("Mouse Wheel to scroll.");
    Pato::BulletText("While editing text:\n");
    Pato::Indent();
    Pato::BulletText("Hold SHIFT or use mouse to select text.");
    Pato::BulletText("CTRL+Left/Right to word jump.");
    Pato::BulletText("CTRL+A or double-click to select all.");
    Pato::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
    Pato::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    Pato::BulletText("ESCAPE to revert.");
    Pato::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    Pato::Unindent();
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window / ShowDemoWindow()
//-----------------------------------------------------------------------------

// We split the contents of the big ShowDemoWindow() function into smaller functions (because the link time of very large functions grow non-linearly)
static void ShowDemoWindowWidgets();
static void ShowDemoWindowLayout();
static void ShowDemoWindowPopups();
static void ShowDemoWindowColumns();
static void ShowDemoWindowMisc();

// Demonstrate most Dear Pato features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does. You may then search for keywords in the code when you are interested by a specific feature.
void Pato::ShowDemoWindow(bool* p_open)
{
    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_documents = false;
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);     // Process the Document app next, as it may also use a DockSpace()
    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    // Dear Pato Apps (accessible from the "Help" menu)
    static bool show_app_metrics = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

    if (show_app_metrics)             { Pato::ShowMetricsWindow(&show_app_metrics); }
    if (show_app_style_editor)        { Pato::Begin("Style Editor", &show_app_style_editor); Pato::ShowStyleEditor(); Pato::End(); }
    if (show_app_about)               { Pato::ShowAboutWindow(&show_app_about); }

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;

    PatoWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= PatoWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= PatoWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= PatoWindowFlags_MenuBar;
    if (no_move)            window_flags |= PatoWindowFlags_NoMove;
    if (no_resize)          window_flags |= PatoWindowFlags_NoResize;
    if (no_collapse)        window_flags |= PatoWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= PatoWindowFlags_NoNav;
    if (no_background)      window_flags |= PatoWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= PatoWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
    Pato::SetNextWindowPos(ImVec2(650, 20), PatoCond_FirstUseEver);
    Pato::SetNextWindowSize(ImVec2(550, 680), PatoCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!Pato::Begin("Pato Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        Pato::End();
        return;
    }
    Pato::Text("dear Pato says hello. (%s)", Pato_VERSION);

    // Most "big" widgets share a common width settings by default.
    //Pato::PushItemWidth(Pato::GetWindowWidth() * 0.65f);    // Use 2/3 of the space for widgets and 1/3 for labels (default)
    Pato::PushItemWidth(Pato::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

    // Menu
    if (Pato::BeginMenuBar())
    {
        if (Pato::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            Pato::EndMenu();
        }
        if (Pato::BeginMenu("Examples"))
        {
            Pato::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            Pato::MenuItem("Console", NULL, &show_app_console);
            Pato::MenuItem("Log", NULL, &show_app_log);
            Pato::MenuItem("Simple layout", NULL, &show_app_layout);
            Pato::MenuItem("Property editor", NULL, &show_app_property_editor);
            Pato::MenuItem("Long text display", NULL, &show_app_long_text);
            Pato::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            Pato::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            Pato::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            Pato::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            Pato::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            Pato::MenuItem("Documents", NULL, &show_app_documents);
            Pato::EndMenu();
        }
        if (Pato::BeginMenu("Help"))
        {
            Pato::MenuItem("Metrics", NULL, &show_app_metrics);
            Pato::MenuItem("Style Editor", NULL, &show_app_style_editor);
            Pato::MenuItem("About Dear Pato", NULL, &show_app_about);
            Pato::EndMenu();
        }
        Pato::EndMenuBar();
    }

    Pato::Spacing();
    if (Pato::CollapsingHeader("Help"))
    {
        Pato::Text("PROGRAMMER GUIDE:");
        Pato::BulletText("Please see the ShowDemoWindow() code in Pato_demo.cpp. <- you are here!");
        Pato::BulletText("Please see the comments in Pato.cpp.");
        Pato::BulletText("Please see the examples/ in application.");
        Pato::BulletText("Enable 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        Pato::BulletText("Enable 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        Pato::Separator();

        Pato::Text("USER GUIDE:");
        Pato::ShowUserGuide();
    }

    if (Pato::CollapsingHeader("Configuration"))
    {
        PatoIO& io = Pato::GetIO();

        if (Pato::TreeNode("Configuration##2"))
        {
            Pato::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", (unsigned int *)&io.ConfigFlags, PatoConfigFlags_NavEnableKeyboard);
            Pato::CheckboxFlags("io.ConfigFlags: NavEnableGamepad", (unsigned int *)&io.ConfigFlags, PatoConfigFlags_NavEnableGamepad);
            Pato::SameLine(); ShowHelpMarker("Required back-end to feed in gamepad inputs in io.NavInputs[] and set io.BackendFlags |= PatoBackendFlags_HasGamepad.\n\nRead instructions in Pato.cpp for details.");
            Pato::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", (unsigned int *)&io.ConfigFlags, PatoConfigFlags_NavEnableSetMousePos);
            Pato::SameLine(); ShowHelpMarker("Instruct navigation to move the mouse cursor. See comment for PatoConfigFlags_NavEnableSetMousePos.");
            Pato::CheckboxFlags("io.ConfigFlags: NoMouse", (unsigned int *)&io.ConfigFlags, PatoConfigFlags_NoMouse);
            if (io.ConfigFlags & PatoConfigFlags_NoMouse) // Create a way to restore this flag otherwise we could be stuck completely!
            {
                if (fmodf((float)Pato::GetTime(), 0.40f) < 0.20f)
                {
                    Pato::SameLine();
                    Pato::Text("<<PRESS SPACE TO DISABLE>>");
                }
                if (Pato::IsKeyPressed(Pato::GetKeyIndex(PatoKey_Space)))
                    io.ConfigFlags &= ~PatoConfigFlags_NoMouse;
            }
            Pato::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", (unsigned int *)&io.ConfigFlags, PatoConfigFlags_NoMouseCursorChange);
            Pato::SameLine(); ShowHelpMarker("Instruct back-end to not alter mouse cursor shape and visibility.");
            Pato::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            Pato::SameLine(); ShowHelpMarker("Set to false to disable blinking cursor, for users who consider it distracting");
            Pato::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            Pato::SameLine(); ShowHelpMarker("Enable resizing of windows from their edges and from the lower-left corner.\nThis requires (io.BackendFlags & PatoBackendFlags_HasMouseCursors) because it needs mouse cursor feedback.");
            Pato::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            Pato::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            Pato::SameLine(); ShowHelpMarker("Instruct Dear Pato to render a mouse cursor for you. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable software cursor only when resizing/dragging something).");
            Pato::TreePop();
            Pato::Separator();
        }

        if (Pato::TreeNode("Backend Flags"))
        {
            PatoBackendFlags backend_flags = io.BackendFlags; // Make a local copy to avoid modifying the back-end flags.
            Pato::CheckboxFlags("io.BackendFlags: HasGamepad", (unsigned int *)&backend_flags, PatoBackendFlags_HasGamepad);
            Pato::CheckboxFlags("io.BackendFlags: HasMouseCursors", (unsigned int *)&backend_flags, PatoBackendFlags_HasMouseCursors);
            Pato::CheckboxFlags("io.BackendFlags: HasSetMousePos", (unsigned int *)&backend_flags, PatoBackendFlags_HasSetMousePos);
            Pato::TreePop();
            Pato::Separator();
        }

        if (Pato::TreeNode("Style"))
        {
            Pato::ShowStyleEditor();
            Pato::TreePop();
            Pato::Separator();
        }

        if (Pato::TreeNode("Capture/Logging"))
        {
            Pato::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded.");
            ShowHelpMarker("Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            Pato::LogButtons();
            Pato::TextWrapped("You can also call Pato::LogText() to output directly to the log without a visual output.");
            if (Pato::Button("Copy \"Hello, world!\" to clipboard"))
            {
                Pato::LogToClipboard();
                Pato::LogText("Hello, world!");
                Pato::LogFinish();
            }
            Pato::TreePop();
        }
    }

    if (Pato::CollapsingHeader("Window options"))
    {
        Pato::Checkbox("No titlebar", &no_titlebar); Pato::SameLine(150);
        Pato::Checkbox("No scrollbar", &no_scrollbar); Pato::SameLine(300);
        Pato::Checkbox("No menu", &no_menu);
        Pato::Checkbox("No move", &no_move); Pato::SameLine(150);
        Pato::Checkbox("No resize", &no_resize); Pato::SameLine(300);
        Pato::Checkbox("No collapse", &no_collapse);
        Pato::Checkbox("No close", &no_close); Pato::SameLine(150);
        Pato::Checkbox("No nav", &no_nav); Pato::SameLine(300);
        Pato::Checkbox("No background", &no_background);
        Pato::Checkbox("No bring to front", &no_bring_to_front);
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowColumns();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    Pato::End();
}

static void ShowDemoWindowWidgets()
{
    if (!Pato::CollapsingHeader("Widgets"))
        return;

    if (Pato::TreeNode("Basic"))
    {
        static int clicked = 0;
        if (Pato::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            Pato::SameLine();
            Pato::Text("Thanks for clicking me!");
        }

        static bool check = true;
        Pato::Checkbox("checkbox", &check);

        static int e = 0;
        Pato::RadioButton("radio a", &e, 0); Pato::SameLine();
        Pato::RadioButton("radio b", &e, 1); Pato::SameLine();
        Pato::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) 
                Pato::SameLine();
            Pato::PushID(i);
            Pato::PushStyleColor(PatoCol_Button, (ImVec4)ImColor::HSV(i/7.0f, 0.6f, 0.6f));
            Pato::PushStyleColor(PatoCol_ButtonHovered, (ImVec4)ImColor::HSV(i/7.0f, 0.7f, 0.7f));
            Pato::PushStyleColor(PatoCol_ButtonActive, (ImVec4)ImColor::HSV(i/7.0f, 0.8f, 0.8f));
            Pato::Button("Click");
            Pato::PopStyleColor(3);
            Pato::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
        Pato::AlignTextToFramePadding();
        Pato::Text("Hold to repeat:");
        Pato::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = Pato::GetStyle().ItemInnerSpacing.x;
        Pato::PushButtonRepeat(true);
        if (Pato::ArrowButton("##left", PatoDir_Left)) { counter--; }
        Pato::SameLine(0.0f, spacing);
        if (Pato::ArrowButton("##right", PatoDir_Right)) { counter++; }
        Pato::PopButtonRepeat();
        Pato::SameLine();
        Pato::Text("%d", counter);

        Pato::Text("Hover over me");
        if (Pato::IsItemHovered())
            Pato::SetTooltip("I am a tooltip");

        Pato::SameLine();
        Pato::Text("- or me");
        if (Pato::IsItemHovered())
        {
            Pato::BeginTooltip();
            Pato::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            Pato::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            Pato::EndTooltip();
        }

        Pato::Separator();

        Pato::LabelText("label", "Value");

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
            static int item_current = 0;
            Pato::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            Pato::SameLine(); ShowHelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");
        }

        {
            static char str0[128] = "Hello, world!";
            static int i0 = 123;
            Pato::InputText("input text", str0, IM_ARRAYSIZE(str0));
            Pato::SameLine(); ShowHelpMarker("USER:\nHold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n\nPROGRAMMER:\nYou can use the PatoInputTextFlags_CallbackResize facility if you need to wire InputText() to a dynamic string type. See misc/cpp/Pato_stdlib.h for an example (this is not demonstrated in Pato_demo.cpp).");

            Pato::InputInt("input int", &i0);
            Pato::SameLine(); ShowHelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

            static float f0 = 0.001f;
            Pato::InputFloat("input float", &f0, 0.01f, 1.0f);

            static double d0 = 999999.00000001;
            Pato::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            Pato::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            Pato::SameLine(); ShowHelpMarker("You can input value using the scientific notation,\n  e.g. \"1e+8\" becomes \"100000000\".\n");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            Pato::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            Pato::DragInt("drag int", &i1, 1);
            Pato::SameLine(); ShowHelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

            Pato::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%");

            static float f1=1.00f, f2=0.0067f;
            Pato::DragFloat("drag float", &f1, 0.005f);
            Pato::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1=0;
            Pato::SliderInt("slider int", &i1, -1, 3);
            Pato::SameLine(); ShowHelpMarker("CTRL+click to input value.");

            static float f1=0.123f, f2=0.0f;
            Pato::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            Pato::SliderFloat("slider float (curve)", &f2, -10.0f, 10.0f, "%.4f", 2.0f);
            static float angle = 0.0f;
            Pato::SliderAngle("slider angle", &angle);
        }

        {
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            Pato::ColorEdit3("color 1", col1);
            Pato::SameLine(); ShowHelpMarker("Click on the colored square to open a color picker.\nClick and hold to use drag and drop.\nRight-click on the colored square to show options.\nCTRL+click on individual component to input value.\n");

            Pato::ColorEdit4("color 2", col2);
        }

        {
            // List box
            const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int listbox_item_current = 1;
            Pato::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

            //static int listbox_item_current2 = 2;
            //Pato::PushItemWidth(-1);
            //Pato::ListBox("##listbox2", &listbox_item_current2, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
            //Pato::PopItemWidth();
        }

        Pato::TreePop();
    }

    // Testing PatoOnceUponAFrame helper.
    //static PatoOnceUponAFrame once;
    //for (int i = 0; i < 5; i++)
    //    if (once)
    //        Pato::Text("This will be displayed only once.");

    if (Pato::TreeNode("Trees"))
    {
        if (Pato::TreeNode("Basic trees"))
        {
            for (int i = 0; i < 5; i++)
                if (Pato::TreeNode((void*)(intptr_t)i, "Child %d", i))
                {
                    Pato::Text("blah blah");
                    Pato::SameLine();
                    if (Pato::SmallButton("button")) { };
                    Pato::TreePop();
                }
            Pato::TreePop();
        }

        if (Pato::TreeNode("Advanced, with Selectable nodes"))
        {
            ShowHelpMarker("This is a more standard looking tree with selectable nodes.\nClick to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static bool align_label_with_current_x_position = false;
            Pato::Checkbox("Align label with current X position)", &align_label_with_current_x_position);
            Pato::Text("Hello!");
            if (align_label_with_current_x_position)
                Pato::Unindent(Pato::GetTreeNodeToLabelSpacing());

            static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
            int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
            Pato::PushStyleVar(PatoStyleVar_IndentSpacing, Pato::GetFontSize()*3); // Increase spacing to differentiate leaves from expanded contents.
            for (int i = 0; i < 6; i++)
            {
                // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
                PatoTreeNodeFlags node_flags = PatoTreeNodeFlags_OpenOnArrow | PatoTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? PatoTreeNodeFlags_Selected : 0);
                if (i < 3)
                {
                    // Node
                    bool node_open = Pato::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (Pato::IsItemClicked())
                        node_clicked = i;
                    if (node_open)
                    {
                        Pato::Text("Blah blah\nBlah Blah");
                        Pato::TreePop();
                    }
                }
                else
                {
                    // Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
                    node_flags |= PatoTreeNodeFlags_Leaf | PatoTreeNodeFlags_NoTreePushOnOpen; // PatoTreeNodeFlags_Bullet
                    Pato::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (Pato::IsItemClicked())
                        node_clicked = i;
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
                if (Pato::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            Pato::PopStyleVar();
            if (align_label_with_current_x_position)
                Pato::Indent(Pato::GetTreeNodeToLabelSpacing());
            Pato::TreePop();
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        Pato::Checkbox("Enable extra group", &closable_group);
        if (Pato::CollapsingHeader("Header"))
        {
            Pato::Text("IsItemHovered: %d", Pato::IsItemHovered());
            for (int i = 0; i < 5; i++)
                Pato::Text("Some content %d", i);
        }
        if (Pato::CollapsingHeader("Header with a close button", &closable_group))
        {
            Pato::Text("IsItemHovered: %d", Pato::IsItemHovered());
            for (int i = 0; i < 5; i++)
                Pato::Text("More content %d", i);
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Bullets"))
    {
        Pato::BulletText("Bullet point 1");
        Pato::BulletText("Bullet point 2\nOn multiple lines");
        Pato::Bullet(); Pato::Text("Bullet point 3 (two calls)");
        Pato::Bullet(); Pato::SmallButton("Button");
        Pato::TreePop();
    }

    if (Pato::TreeNode("Text"))
    {
        if (Pato::TreeNode("Colored Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            Pato::TextColored(ImVec4(1.0f,0.0f,1.0f,1.0f), "Pink");
            Pato::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "Yellow");
            Pato::TextDisabled("Disabled");
            Pato::SameLine(); ShowHelpMarker("The TextDisabled color is stored in PatoStyle.");
            Pato::TreePop();
        }

        if (Pato::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            Pato::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
            Pato::Spacing();

            static float wrap_width = 200.0f;
            Pato::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            Pato::Text("Test paragraph 1:");
            ImVec2 pos = Pato::GetCursorScreenPos();
            Pato::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + Pato::GetTextLineHeight()), IM_COL32(255,0,255,255));
            Pato::PushTextWrapPos(Pato::GetCursorPos().x + wrap_width);
            Pato::Text("The lazy dog is a good dog. This paragraph is made to fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
            Pato::GetWindowDrawList()->AddRect(Pato::GetItemRectMin(), Pato::GetItemRectMax(), IM_COL32(255,255,0,255));
            Pato::PopTextWrapPos();

            Pato::Text("Test paragraph 2:");
            pos = Pato::GetCursorScreenPos();
            Pato::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + Pato::GetTextLineHeight()), IM_COL32(255,0,255,255));
            Pato::PushTextWrapPos(Pato::GetCursorPos().x + wrap_width);
            Pato::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
            Pato::GetWindowDrawList()->AddRect(Pato::GetItemRectMin(), Pato::GetItemRectMax(), IM_COL32(255,255,0,255));
            Pato::PopTextWrapPos();

            Pato::TreePop();
        }

        if (Pato::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (Needs a suitable font, try Noto, or Arial Unicode, or M+ fonts. Read misc/fonts/README.txt for details.)
            // - From C++11 you can use the u8"my text" syntax to encode literal strings as UTF-8
            // - For earlier compiler, you may be able to encode your sources as UTF-8 (e.g. Visual Studio save your file as 'UTF-8 without signature')
            // - FOR THIS DEMO FILE ONLY, BECAUSE WE WANT TO SUPPORT OLD COMPILERS, WE ARE *NOT* INCLUDING RAW UTF-8 CHARACTERS IN THIS SOURCE FILE.
            //   Instead we are encoding a few strings with hexadecimal constants. Don't do this in your application!
            //   Please use u8"text in any language" in your application!
            // Note that characters values are preserved even by InputText() if the font cannot be displayed, so you can safely copy & paste garbled characters into another application.
            Pato::TextWrapped("CJK text will only appears if the font was loaded with the appropriate CJK character ranges. Call io.Font->AddFontFromFileTTF() manually to load extra character ranges. Read misc/fonts/README.txt for details.");
            Pato::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)"); // Normally we would use u8"blah blah" with the proper characters directly in the string.
            Pato::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            //static char buf[32] = u8"NIHONGO"; // <- this is how you would write it with C++11, using real kanjis
            Pato::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            Pato::TreePop();
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Images"))
    {
        PatoIO& io = Pato::GetIO();
        Pato::TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");

        // Here we are grabbing the font texture because that's the only one we have access to inside the demo code.
        // Remember that ImTextureID is just storage for whatever you want it to be, it is essentially a value that will be passed to the render function inside the ImDrawCmd structure.
        // If you use one of the default Pato_impl_XXXX.cpp renderer, they all have comments at the top of their file to specify what they expect to be stored in ImTextureID.
        // (for example, the Pato_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer. The Pato_impl_glfw_gl3.cpp renderer expect a GLuint OpenGL texture identifier etc.)
        // If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers to Pato::Image(), and gather width/height through your own functions, etc.
        // Using ShowMetricsWindow() as a "debugger" to inspect the draw data that are being passed to your render will help you debug issues if you are confused about this.
        // Consider using the lower-level ImDrawList::AddImage() API, via Pato::GetWindowDrawList()->AddImage().
        ImTextureID my_tex_id = io.Fonts->TexID;
        float my_tex_w = (float)io.Fonts->TexWidth;
        float my_tex_h = (float)io.Fonts->TexHeight;

        Pato::Text("%.0fx%.0f", my_tex_w, my_tex_h);
        ImVec2 pos = Pato::GetCursorScreenPos();
        Pato::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
        if (Pato::IsItemHovered())
        {
            Pato::BeginTooltip();
            float region_sz = 32.0f;
            float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > my_tex_w - region_sz) region_x = my_tex_w - region_sz;
            float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > my_tex_h - region_sz) region_y = my_tex_h - region_sz;
            float zoom = 4.0f;
            Pato::Text("Min: (%.2f, %.2f)", region_x, region_y);
            Pato::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
            ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
            ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
            Pato::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
            Pato::EndTooltip();
        }
        Pato::TextWrapped("And now some textured buttons..");
        static int pressed_count = 0;
        for (int i = 0; i < 8; i++)
        {
            Pato::PushID(i);
            int frame_padding = -1 + i;     // -1 = uses default padding
            if (Pato::ImageButton(my_tex_id, ImVec2(32,32), ImVec2(0,0), ImVec2(32.0f/my_tex_w,32/my_tex_h), frame_padding, ImColor(0,0,0,255)))
                pressed_count += 1;
            Pato::PopID();
            Pato::SameLine();
        }
        Pato::NewLine();
        Pato::Text("Pressed %d times.", pressed_count);
        Pato::TreePop();
    }

    if (Pato::TreeNode("Combo"))
    {
        // Expose flags as checkbox for the demo
        static PatoComboFlags flags = 0;
        Pato::CheckboxFlags("PatoComboFlags_PopupAlignLeft", (unsigned int*)&flags, PatoComboFlags_PopupAlignLeft);
        if (Pato::CheckboxFlags("PatoComboFlags_NoArrowButton", (unsigned int*)&flags, PatoComboFlags_NoArrowButton))
            flags &= ~PatoComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (Pato::CheckboxFlags("PatoComboFlags_NoPreview", (unsigned int*)&flags, PatoComboFlags_NoPreview))
            flags &= ~PatoComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        // General BeginCombo() API, you have full control over your selection data and display type.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
        if (Pato::BeginCombo("combo 1", item_current, flags)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (item_current == items[n]);
                if (Pato::Selectable(items[n], is_selected))
                    item_current = items[n];
                if (is_selected)
                    Pato::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            Pato::EndCombo();
        }

        // Simplified one-liner Combo() API, using values packed in a single constant string
        static int item_current_2 = 0;
        Pato::Combo("combo 2 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        // Simplified one-liner Combo() using an array of const char*
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        Pato::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));

        // Simplified one-liner Combo() using an accessor function
        struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
        static int item_current_4 = 0;
        Pato::Combo("combo 4 (function)", &item_current_4, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

        Pato::TreePop();
    }

    if (Pato::TreeNode("Selectables"))
    {
        // Selectable() has 2 overloads:
        // - The one taking "bool selected" as a read-only selection information. When Selectable() has been clicked is returns true and you can alter selection state accordingly.
        // - The one taking "bool* p_selected" as a read-write selection information (convenient in some cases)
        // The earlier is more flexible, as in real application your selection may be stored in a different manner (in flags within objects, as an external list, etc).
        if (Pato::TreeNode("Basic"))
        {
            static bool selection[5] = { false, true, false, false, false };
            Pato::Selectable("1. I am selectable", &selection[0]);
            Pato::Selectable("2. I am selectable", &selection[1]);
            Pato::Text("3. I am not selectable");
            Pato::Selectable("4. I am selectable", &selection[3]);
            if (Pato::Selectable("5. I am double clickable", selection[4], PatoSelectableFlags_AllowDoubleClick))
                if (Pato::IsMouseDoubleClicked(0))
                    selection[4] = !selection[4];
            Pato::TreePop();
        }
        if (Pato::TreeNode("Selection State: Single Selection"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (Pato::Selectable(buf, selected == n))
                    selected = n;
            }
            Pato::TreePop();
        }
        if (Pato::TreeNode("Selection State: Multiple Selection"))
        {
            ShowHelpMarker("Hold CTRL and click to select multiple items.");
            static bool selection[5] = { false, false, false, false, false };
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (Pato::Selectable(buf, selection[n]))
                {
                    if (!Pato::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                        memset(selection, 0, sizeof(selection));
                    selection[n] ^= 1;
                }
            }
            Pato::TreePop();
        }
        if (Pato::TreeNode("Rendering more text into the same line"))
        {
            // Using the Selectable() override that takes "bool* p_selected" parameter and toggle your booleans automatically.
            static bool selected[3] = { false, false, false };
            Pato::Selectable("main.c",    &selected[0]); Pato::SameLine(300); Pato::Text(" 2,345 bytes");
            Pato::Selectable("Hello.cpp", &selected[1]); Pato::SameLine(300); Pato::Text("12,345 bytes");
            Pato::Selectable("Hello.h",   &selected[2]); Pato::SameLine(300); Pato::Text(" 2,345 bytes");
            Pato::TreePop();
        }
        if (Pato::TreeNode("In columns"))
        {
            Pato::Columns(3, NULL, false);
            static bool selected[16] = { 0 };
            for (int i = 0; i < 16; i++)
            {
                char label[32]; sprintf(label, "Item %d", i);
                if (Pato::Selectable(label, &selected[i])) {}
                Pato::NextColumn();
            }
            Pato::Columns(1);
            Pato::TreePop();
        }
        if (Pato::TreeNode("Grid"))
        {
            static bool selected[16] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
            for (int i = 0; i < 16; i++)
            {
                Pato::PushID(i);
                if (Pato::Selectable("Sailor", &selected[i], 0, ImVec2(50,50)))
                {
                    int x = i % 4, y = i / 4;
                    if (x > 0) selected[i - 1] ^= 1;
                    if (x < 3) selected[i + 1] ^= 1;
                    if (y > 0) selected[i - 4] ^= 1;
                    if (y < 3) selected[i + 4] ^= 1;
                }
                if ((i % 4) < 3) Pato::SameLine();
                Pato::PopID();
            }
            Pato::TreePop();
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Filtered Text Input"))
    {
        static char buf1[64] = ""; Pato::InputText("default", buf1, 64);
        static char buf2[64] = ""; Pato::InputText("decimal", buf2, 64, PatoInputTextFlags_CharsDecimal);
        static char buf3[64] = ""; Pato::InputText("hexadecimal", buf3, 64, PatoInputTextFlags_CharsHexadecimal | PatoInputTextFlags_CharsUppercase);
        static char buf4[64] = ""; Pato::InputText("uppercase", buf4, 64, PatoInputTextFlags_CharsUppercase);
        static char buf5[64] = ""; Pato::InputText("no blank", buf5, 64, PatoInputTextFlags_CharsNoBlank);
        struct TextFilters { static int FilterPatoLetters(PatoInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("Pato", (char)data->EventChar)) return 0; return 1; } };
        static char buf6[64] = ""; Pato::InputText("\"Pato\" letters", buf6, 64, PatoInputTextFlags_CallbackCharFilter, TextFilters::FilterPatoLetters);

        Pato::Text("Password input");
        static char bufpass[64] = "password123";
        Pato::InputText("password", bufpass, 64, PatoInputTextFlags_Password | PatoInputTextFlags_CharsNoBlank);
        Pato::SameLine(); ShowHelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
        Pato::InputText("password (clear)", bufpass, 64, PatoInputTextFlags_CharsNoBlank);

        Pato::TreePop();
    }

    if (Pato::TreeNode("Multi-line Text Input"))
    {
        // Note: we are using a fixed-sized buffer for simplicity here. See PatoInputTextFlags_CallbackResize 
        // and the code in misc/cpp/Pato_stdlib.h for how to setup InputText() for dynamically resizing strings.
        static bool read_only = false;
        static char text[1024*16] =
            "/*\n"
            " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
            " the hexadecimal encoding of one offending instruction,\n"
            " more formally, the invalid operand with locked CMPXCHG8B\n"
            " instruction bug, is a design flaw in the majority of\n"
            " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
            " processors (all in the P5 microarchitecture).\n"
            "*/\n\n"
            "label:\n"
            "\tlock cmpxchg8b eax\n";

        ShowHelpMarker("You can use the PatoInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/Pato_stdlib.h for an example. (This is not demonstrated in Pato_demo.cpp)");
        Pato::Checkbox("Read-only", &read_only);
        PatoInputTextFlags flags = PatoInputTextFlags_AllowTabInput | (read_only ? PatoInputTextFlags_ReadOnly : 0);
        Pato::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.0f, Pato::GetTextLineHeight() * 16), flags);
        Pato::TreePop();
    }

    if (Pato::TreeNode("Plots Widgets"))
    {
        static bool animate = true;
        Pato::Checkbox("Animate", &animate);

        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        Pato::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        // Create a dummy array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
        static float values[90] = { 0 };
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0f)
            refresh_time = Pato::GetTime();
        while (refresh_time < Pato::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset+1) % IM_ARRAYSIZE(values);
            phase += 0.10f*values_offset;
            refresh_time += 1.0f/60.0f;
        }
        Pato::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0,80));
        Pato::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0,80));

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        Pato::Separator();
        Pato::PushItemWidth(100); Pato::Combo("func", &func_type, "Sin\0Saw\0"); Pato::PopItemWidth();
        Pato::SameLine();
        Pato::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        Pato::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0,80));
        Pato::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0,80));
        Pato::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * Pato::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        Pato::ProgressBar(progress, ImVec2(0.0f,0.0f));
        Pato::SameLine(0.0f, Pato::GetStyle().ItemInnerSpacing.x);
        Pato::Text("Progress Bar");

        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated*1753), 1753);
        Pato::ProgressBar(progress, ImVec2(0.f,0.f), buf);
        Pato::TreePop();
    }

    if (Pato::TreeNode("Color/Picker Widgets"))
    {
        static ImVec4 color = ImColor(114, 144, 154, 200);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        Pato::Checkbox("With Alpha Preview", &alpha_preview);
        Pato::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        Pato::Checkbox("With Drag and Drop", &drag_and_drop);
        Pato::Checkbox("With Options Menu", &options_menu); Pato::SameLine(); ShowHelpMarker("Right-click on the individual color widget to show options.");
        Pato::Checkbox("With HDR", &hdr); Pato::SameLine(); ShowHelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
        int misc_flags = (hdr ? PatoColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : PatoColorEditFlags_NoDragDrop) | (alpha_half_preview ? PatoColorEditFlags_AlphaPreviewHalf : (alpha_preview ? PatoColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : PatoColorEditFlags_NoOptions);

        Pato::Text("Color widget:");
        Pato::SameLine(); ShowHelpMarker("Click on the colored square to open a color picker.\nCTRL+click on individual component to input value.\n");
        Pato::ColorEdit3("MyColor##1", (float*)&color, misc_flags);

        Pato::Text("Color widget HSV with Alpha:");
        Pato::ColorEdit4("MyColor##2", (float*)&color, PatoColorEditFlags_HSV | misc_flags);

        Pato::Text("Color widget with Float Display:");
        Pato::ColorEdit4("MyColor##2f", (float*)&color, PatoColorEditFlags_Float | misc_flags);

        Pato::Text("Color button with Picker:");
        Pato::SameLine(); ShowHelpMarker("With the PatoColorEditFlags_NoInputs flag you can hide all the slider/text inputs.\nWith the PatoColorEditFlags_NoLabel flag you can pass a non-empty label which will only be used for the tooltip and picker popup.");
        Pato::ColorEdit4("MyColor##3", (float*)&color, PatoColorEditFlags_NoInputs | PatoColorEditFlags_NoLabel | misc_flags);

        Pato::Text("Color button with Custom Picker Popup:");

        // Generate a dummy palette
        static bool saved_palette_inited = false;
        static ImVec4 saved_palette[32];
        if (!saved_palette_inited)
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                Pato::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
        saved_palette_inited = true;

        static ImVec4 backup_color;
        bool open_popup = Pato::ColorButton("MyColor##3b", color, misc_flags);
        Pato::SameLine();
        open_popup |= Pato::Button("Palette");
        if (open_popup)
        {
            Pato::OpenPopup("mypicker");
            backup_color = color;
        }
        if (Pato::BeginPopup("mypicker"))
        {
            // FIXME: Adding a drag and drop example here would be perfect!
            Pato::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            Pato::Separator();
            Pato::ColorPicker4("##picker", (float*)&color, misc_flags | PatoColorEditFlags_NoSidePreview | PatoColorEditFlags_NoSmallPreview);
            Pato::SameLine();
            Pato::BeginGroup();
            Pato::Text("Current");
            Pato::ColorButton("##current", color, PatoColorEditFlags_NoPicker | PatoColorEditFlags_AlphaPreviewHalf, ImVec2(60,40));
            Pato::Text("Previous");
            if (Pato::ColorButton("##previous", backup_color, PatoColorEditFlags_NoPicker | PatoColorEditFlags_AlphaPreviewHalf, ImVec2(60,40)))
                color = backup_color;
            Pato::Separator();
            Pato::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                Pato::PushID(n);
                if ((n % 8) != 0)
                    Pato::SameLine(0.0f, Pato::GetStyle().ItemSpacing.y);
                if (Pato::ColorButton("##palette", saved_palette[n], PatoColorEditFlags_NoAlpha | PatoColorEditFlags_NoPicker | PatoColorEditFlags_NoTooltip, ImVec2(20,20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                if (Pato::BeginDragDropTarget())
                {
                    if (const PatoPayload* payload = Pato::AcceptDragDropPayload(Pato_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const PatoPayload* payload = Pato::AcceptDragDropPayload(Pato_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                    Pato::EndDragDropTarget();
                }

                Pato::PopID();
            }
            Pato::EndGroup();
            Pato::EndPopup();
        }

        Pato::Text("Color button only:");
        Pato::ColorButton("MyColor##3c", *(ImVec4*)&color, misc_flags, ImVec2(80,80));

        Pato::Text("Color picker:");
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static ImVec4 ref_color_v(1.0f,0.0f,1.0f,0.5f);
        static int inputs_mode = 2;
        static int picker_mode = 0;
        Pato::Checkbox("With Alpha", &alpha);
        Pato::Checkbox("With Alpha Bar", &alpha_bar);
        Pato::Checkbox("With Side Preview", &side_preview);
        if (side_preview)
        {
            Pato::SameLine();
            Pato::Checkbox("With Ref Color", &ref_color);
            if (ref_color)
            {
                Pato::SameLine();
                Pato::ColorEdit4("##RefColor", &ref_color_v.x, PatoColorEditFlags_NoInputs | misc_flags);
            }
        }
        Pato::Combo("Inputs Mode", &inputs_mode, "All Inputs\0No Inputs\0RGB Input\0HSV Input\0HEX Input\0");
        Pato::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
        Pato::SameLine(); ShowHelpMarker("User can right-click the picker to change mode.");
        PatoColorEditFlags flags = misc_flags;
        if (!alpha) flags |= PatoColorEditFlags_NoAlpha; // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar) flags |= PatoColorEditFlags_AlphaBar;
        if (!side_preview) flags |= PatoColorEditFlags_NoSidePreview;
        if (picker_mode == 1) flags |= PatoColorEditFlags_PickerHueBar;
        if (picker_mode == 2) flags |= PatoColorEditFlags_PickerHueWheel;
        if (inputs_mode == 1) flags |= PatoColorEditFlags_NoInputs;
        if (inputs_mode == 2) flags |= PatoColorEditFlags_RGB;
        if (inputs_mode == 3) flags |= PatoColorEditFlags_HSV;
        if (inputs_mode == 4) flags |= PatoColorEditFlags_HEX;
        Pato::ColorPicker4("MyColor##4", (float*)&color, flags, ref_color ? &ref_color_v.x : NULL);

        Pato::Text("Programmatically set defaults:");
        Pato::SameLine(); ShowHelpMarker("SetColorEditOptions() is designed to allow you to set boot-time default.\nWe don't have Push/Pop functions because you can force options on a per-widget basis if needed, and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid encouraging you to persistently save values that aren't forward-compatible.");
        if (Pato::Button("Default: Uint8 + HSV + Hue Bar"))
            Pato::SetColorEditOptions(PatoColorEditFlags_Uint8 | PatoColorEditFlags_HSV | PatoColorEditFlags_PickerHueBar);
        if (Pato::Button("Default: Float + HDR + Hue Wheel"))
            Pato::SetColorEditOptions(PatoColorEditFlags_Float | PatoColorEditFlags_HDR | PatoColorEditFlags_PickerHueWheel);

        Pato::TreePop();
    }

    if (Pato::TreeNode("Range Widgets"))
    {
        static float begin = 10, end = 90;
        static int begin_i = 100, end_i = 1000;
        Pato::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
        Pato::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %d units", "Max: %d units");
        Pato::TreePop();
    }

    if (Pato::TreeNode("Data Types"))
    {
        // The DragScalar/InputScalar/SliderScalar functions allow various data types: signed/unsigned int/long long and float/double
        // To avoid polluting the public API with all possible combinations, we use the PatoDataType enum to pass the type, 
        // and passing all arguments by address. 
        // This is the reason the test code below creates local variables to hold "zero" "one" etc. for each types.
        // In practice, if you frequently use a given type that is not covered by the normal API entry points, you can wrap it 
        // yourself inside a 1 line function which can take typed argument as value instead of void*, and then pass their address 
        // to the generic function. For example:
        //   bool MySliderU64(const char *label, u64* value, u64 min = 0, u64 max = 0, const char* format = "%lld") 
        //   { 
        //      return SliderScalar(label, PatoDataType_U64, value, &min, &max, format); 
        //   }

        // Limits (as helper variables that we can take the address of)
        // Note that the SliderScalar function has a maximum usable range of half the natural type maximum, hence the /2 below.
        #ifndef LLONG_MIN
        ImS64 LLONG_MIN = -9223372036854775807LL - 1;
        ImS64 LLONG_MAX = 9223372036854775807LL;
        ImU64 ULLONG_MAX = (2ULL * 9223372036854775807LL + 1);
        #endif
        const ImS32   s32_zero = 0,   s32_one = 1,   s32_fifty = 50, s32_min = INT_MIN/2,   s32_max = INT_MAX/2,    s32_hi_a = INT_MAX/2 - 100,    s32_hi_b = INT_MAX/2;
        const ImU32   u32_zero = 0,   u32_one = 1,   u32_fifty = 50, u32_min = 0,           u32_max = UINT_MAX/2,   u32_hi_a = UINT_MAX/2 - 100,   u32_hi_b = UINT_MAX/2;
        const ImS64   s64_zero = 0,   s64_one = 1,   s64_fifty = 50, s64_min = LLONG_MIN/2, s64_max = LLONG_MAX/2,  s64_hi_a = LLONG_MAX/2 - 100,  s64_hi_b = LLONG_MAX/2;
        const ImU64   u64_zero = 0,   u64_one = 1,   u64_fifty = 50, u64_min = 0,           u64_max = ULLONG_MAX/2, u64_hi_a = ULLONG_MAX/2 - 100, u64_hi_b = ULLONG_MAX/2;
        const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        const double  f64_zero = 0.,  f64_one = 1.,  f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

        // State
        static ImS32  s32_v = -1;
        static ImU32  u32_v = (ImU32)-1;
        static ImS64  s64_v = -1;
        static ImU64  u64_v = (ImU64)-1;
        static float  f32_v = 0.123f;
        static double f64_v = 90000.01234567890123456789;

        const float drag_speed = 0.2f;
        static bool drag_clamp = false;
        Pato::Text("Drags:");
        Pato::Checkbox("Clamp integers to 0..50", &drag_clamp); Pato::SameLine(); ShowHelpMarker("As with every widgets in dear Pato, we never modify values unless there is a user interaction.\nYou can override the clamping limits by using CTRL+Click to input a value.");
        Pato::DragScalar("drag s32",       PatoDataType_S32,    &s32_v, drag_speed, drag_clamp ? &s32_zero : NULL, drag_clamp ? &s32_fifty : NULL);
        Pato::DragScalar("drag u32",       PatoDataType_U32,    &u32_v, drag_speed, drag_clamp ? &u32_zero : NULL, drag_clamp ? &u32_fifty : NULL, "%u ms");
        Pato::DragScalar("drag s64",       PatoDataType_S64,    &s64_v, drag_speed, drag_clamp ? &s64_zero : NULL, drag_clamp ? &s64_fifty : NULL);
        Pato::DragScalar("drag u64",       PatoDataType_U64,    &u64_v, drag_speed, drag_clamp ? &u64_zero : NULL, drag_clamp ? &u64_fifty : NULL);
        Pato::DragScalar("drag float",     PatoDataType_Float,  &f32_v, 0.005f,  &f32_zero, &f32_one, "%f", 1.0f);
        Pato::DragScalar("drag float ^2",  PatoDataType_Float,  &f32_v, 0.005f,  &f32_zero, &f32_one, "%f", 2.0f); Pato::SameLine(); ShowHelpMarker("You can use the 'power' parameter to increase tweaking precision on one side of the range.");
        Pato::DragScalar("drag double",    PatoDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL,     "%.10f grams", 1.0f);
        Pato::DragScalar("drag double ^2", PatoDataType_Double, &f64_v, 0.0005f, &f64_zero, &f64_one, "0 < %.10f < 1", 2.0f);

        Pato::Text("Sliders");
        Pato::SliderScalar("slider s32 low",     PatoDataType_S32,    &s32_v, &s32_zero, &s32_fifty,"%d");
        Pato::SliderScalar("slider s32 high",    PatoDataType_S32,    &s32_v, &s32_hi_a, &s32_hi_b, "%d");
        Pato::SliderScalar("slider s32 full",    PatoDataType_S32,    &s32_v, &s32_min,  &s32_max,  "%d");
        Pato::SliderScalar("slider u32 low",     PatoDataType_U32,    &u32_v, &u32_zero, &u32_fifty,"%u");
        Pato::SliderScalar("slider u32 high",    PatoDataType_U32,    &u32_v, &u32_hi_a, &u32_hi_b, "%u");
        Pato::SliderScalar("slider u32 full",    PatoDataType_U32,    &u32_v, &u32_min,  &u32_max,  "%u");
        Pato::SliderScalar("slider s64 low",     PatoDataType_S64,    &s64_v, &s64_zero, &s64_fifty,"%I64d");
        Pato::SliderScalar("slider s64 high",    PatoDataType_S64,    &s64_v, &s64_hi_a, &s64_hi_b, "%I64d");
        Pato::SliderScalar("slider s64 full",    PatoDataType_S64,    &s64_v, &s64_min,  &s64_max,  "%I64d");
        Pato::SliderScalar("slider u64 low",     PatoDataType_U64,    &u64_v, &u64_zero, &u64_fifty,"%I64u ms");
        Pato::SliderScalar("slider u64 high",    PatoDataType_U64,    &u64_v, &u64_hi_a, &u64_hi_b, "%I64u ms");
        Pato::SliderScalar("slider u64 full",    PatoDataType_U64,    &u64_v, &u64_min,  &u64_max,  "%I64u ms");
        Pato::SliderScalar("slider float low",   PatoDataType_Float,  &f32_v, &f32_zero, &f32_one);
        Pato::SliderScalar("slider float low^2", PatoDataType_Float,  &f32_v, &f32_zero, &f32_one,  "%.10f", 2.0f);
        Pato::SliderScalar("slider float high",  PatoDataType_Float,  &f32_v, &f32_lo_a, &f32_hi_a, "%e");
        Pato::SliderScalar("slider double low",  PatoDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f grams", 1.0f);
        Pato::SliderScalar("slider double low^2",PatoDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f", 2.0f);
        Pato::SliderScalar("slider double high", PatoDataType_Double, &f64_v, &f64_lo_a, &f64_hi_a, "%e grams", 1.0f);

        static bool inputs_step = true;
        Pato::Text("Inputs");
        Pato::Checkbox("Show step buttons", &inputs_step);
        Pato::InputScalar("input s32",     PatoDataType_S32,    &s32_v, inputs_step ? &s32_one : NULL, NULL, "%d");
        Pato::InputScalar("input s32 hex", PatoDataType_S32,    &s32_v, inputs_step ? &s32_one : NULL, NULL, "%08X", PatoInputTextFlags_CharsHexadecimal);
        Pato::InputScalar("input u32",     PatoDataType_U32,    &u32_v, inputs_step ? &u32_one : NULL, NULL, "%u");
        Pato::InputScalar("input u32 hex", PatoDataType_U32,    &u32_v, inputs_step ? &u32_one : NULL, NULL, "%08X", PatoInputTextFlags_CharsHexadecimal);
        Pato::InputScalar("input s64",     PatoDataType_S64,    &s64_v, inputs_step ? &s64_one : NULL);
        Pato::InputScalar("input u64",     PatoDataType_U64,    &u64_v, inputs_step ? &u64_one : NULL);
        Pato::InputScalar("input float",   PatoDataType_Float,  &f32_v, inputs_step ? &f32_one : NULL);
        Pato::InputScalar("input double",  PatoDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

        Pato::TreePop();
    }

    if (Pato::TreeNode("Multi-component Widgets"))
    {
        static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        Pato::InputFloat2("input float2", vec4f);
        Pato::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
        Pato::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
        Pato::InputInt2("input int2", vec4i);
        Pato::DragInt2("drag int2", vec4i, 1, 0, 255);
        Pato::SliderInt2("slider int2", vec4i, 0, 255);
        Pato::Spacing();

        Pato::InputFloat3("input float3", vec4f);
        Pato::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
        Pato::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
        Pato::InputInt3("input int3", vec4i);
        Pato::DragInt3("drag int3", vec4i, 1, 0, 255);
        Pato::SliderInt3("slider int3", vec4i, 0, 255);
        Pato::Spacing();

        Pato::InputFloat4("input float4", vec4f);
        Pato::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
        Pato::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
        Pato::InputInt4("input int4", vec4i);
        Pato::DragInt4("drag int4", vec4i, 1, 0, 255);
        Pato::SliderInt4("slider int4", vec4i, 0, 255);

        Pato::TreePop();
    }

    if (Pato::TreeNode("Vertical Sliders"))
    {
        const float spacing = 4;
        Pato::PushStyleVar(PatoStyleVar_ItemSpacing, ImVec2(spacing, spacing));

        static int int_value = 0;
        Pato::VSliderInt("##int", ImVec2(18,160), &int_value, 0, 5);
        Pato::SameLine();

        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
        Pato::PushID("set1");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) Pato::SameLine();
            Pato::PushID(i);
            Pato::PushStyleColor(PatoCol_FrameBg, (ImVec4)ImColor::HSV(i/7.0f, 0.5f, 0.5f));
            Pato::PushStyleColor(PatoCol_FrameBgHovered, (ImVec4)ImColor::HSV(i/7.0f, 0.6f, 0.5f));
            Pato::PushStyleColor(PatoCol_FrameBgActive, (ImVec4)ImColor::HSV(i/7.0f, 0.7f, 0.5f));
            Pato::PushStyleColor(PatoCol_SliderGrab, (ImVec4)ImColor::HSV(i/7.0f, 0.9f, 0.9f));
            Pato::VSliderFloat("##v", ImVec2(18,160), &values[i], 0.0f, 1.0f, "");
            if (Pato::IsItemActive() || Pato::IsItemHovered())
                Pato::SetTooltip("%.3f", values[i]);
            Pato::PopStyleColor(4);
            Pato::PopID();
        }
        Pato::PopID();

        Pato::SameLine();
        Pato::PushID("set2");
        static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
        const int rows = 3;
        const ImVec2 small_slider_size(18, (160.0f-(rows-1)*spacing)/rows);
        for (int nx = 0; nx < 4; nx++)
        {
            if (nx > 0) Pato::SameLine();
            Pato::BeginGroup();
            for (int ny = 0; ny < rows; ny++)
            {
                Pato::PushID(nx*rows+ny);
                Pato::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                if (Pato::IsItemActive() || Pato::IsItemHovered())
                    Pato::SetTooltip("%.3f", values2[nx]);
                Pato::PopID();
            }
            Pato::EndGroup();
        }
        Pato::PopID();

        Pato::SameLine();
        Pato::PushID("set3");
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) Pato::SameLine();
            Pato::PushID(i);
            Pato::PushStyleVar(PatoStyleVar_GrabMinSize, 40);
            Pato::VSliderFloat("##v", ImVec2(40,160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
            Pato::PopStyleVar();
            Pato::PopID();
        }
        Pato::PopID();
        Pato::PopStyleVar();
        Pato::TreePop();
    }

    if (Pato::TreeNode("Drag and Drop"))
    {
        {
            // ColorEdit widgets automatically act as drag source and drag target.
            // They are using standardized payload strings Pato_PAYLOAD_TYPE_COLOR_3F and Pato_PAYLOAD_TYPE_COLOR_4F to allow your own widgets
            // to use colors in their drag and drop interaction. Also see the demo in Color Picker -> Palette demo.
            Pato::BulletText("Drag and drop in standard widgets");
            Pato::Indent();
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            Pato::ColorEdit3("color 1", col1);
            Pato::ColorEdit4("color 2", col2);
            Pato::Unindent();
        }

        {
            Pato::BulletText("Drag and drop to copy/swap items");
            Pato::Indent();
            enum Mode
            {
                Mode_Copy,
                Mode_Move,
                Mode_Swap
            };
            static int mode = 0;
            if (Pato::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } Pato::SameLine();
            if (Pato::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } Pato::SameLine();
            if (Pato::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; } 
            static const char* names[9] = { "Bobby", "Beatrice", "Betty", "Brianna", "Barry", "Bernard", "Bibi", "Blaine", "Bryn" };
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                Pato::PushID(n);
                if ((n % 3) != 0)
                    Pato::SameLine();
                Pato::Button(names[n], ImVec2(60,60));

                // Our buttons are both drag sources and drag targets here!
                if (Pato::BeginDragDropSource(PatoDragDropFlags_None))
                {
                    Pato::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));        // Set payload to carry the index of our item (could be anything)
                    if (mode == Mode_Copy) { Pato::Text("Copy %s", names[n]); }        // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
                    if (mode == Mode_Move) { Pato::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { Pato::Text("Swap %s", names[n]); }
                    Pato::EndDragDropSource();
                }
                if (Pato::BeginDragDropTarget())
                {
                    if (const PatoPayload* payload = Pato::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    Pato::EndDragDropTarget();
                }
                Pato::PopID();
            }
            Pato::Unindent();
        }

        Pato::TreePop();
    }

    if (Pato::TreeNode("Querying Status (Active/Focused/Hovered etc.)"))
    {
        // Display the value of IsItemHovered() and other common item state functions. Note that the flags can be combined.
        // (because BulletText is an item itself and that would affect the output of IsItemHovered() we pass all state in a single call to simplify the code).
        static int item_type = 1;
        static bool b = false;
        static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
        Pato::RadioButton("Text", &item_type, 0);
        Pato::RadioButton("Button", &item_type, 1);
        Pato::RadioButton("CheckBox", &item_type, 2);
        Pato::RadioButton("SliderFloat", &item_type, 3);
        Pato::RadioButton("ColorEdit4", &item_type, 4);
        Pato::RadioButton("ListBox", &item_type, 5);
        Pato::Separator();
        bool ret = false;
        if (item_type == 0) { Pato::Text("ITEM: Text"); }                                              // Testing text items with no identifier/interaction
        if (item_type == 1) { ret = Pato::Button("ITEM: Button"); }                                    // Testing button
        if (item_type == 2) { ret = Pato::Checkbox("ITEM: CheckBox", &b); }                            // Testing checkbox
        if (item_type == 3) { ret = Pato::SliderFloat("ITEM: SliderFloat", &col4f[0], 0.0f, 1.0f); }   // Testing basic item
        if (item_type == 4) { ret = Pato::ColorEdit4("ITEM: ColorEdit4", col4f); }                     // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 5) { const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = Pato::ListBox("ITEM: ListBox", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)); }
        Pato::BulletText(
            "Return value = %d\n"
            "IsItemFocused() = %d\n"
            "IsItemHovered() = %d\n"
            "IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsItemHovered(_AllowWhenOverlapped) = %d\n"
            "IsItemHovered(_RectOnly) = %d\n"
            "IsItemActive() = %d\n"
            "IsItemEdited() = %d\n"
            "IsItemDeactivated() = %d\n"
            "IsItemDeactivatedEdit() = %d\n"
            "IsItemVisible() = %d\n"
            "GetItemRectMin() = (%.1f, %.1f)\n"
            "GetItemRectMax() = (%.1f, %.1f)\n"
            "GetItemRectSize() = (%.1f, %.1f)",
            ret,
            Pato::IsItemFocused(),
            Pato::IsItemHovered(),
            Pato::IsItemHovered(PatoHoveredFlags_AllowWhenBlockedByPopup),
            Pato::IsItemHovered(PatoHoveredFlags_AllowWhenBlockedByActiveItem),
            Pato::IsItemHovered(PatoHoveredFlags_AllowWhenOverlapped),
            Pato::IsItemHovered(PatoHoveredFlags_RectOnly),
            Pato::IsItemActive(),
            Pato::IsItemEdited(),
            Pato::IsItemDeactivated(),
            Pato::IsItemDeactivatedAfterEdit(),
            Pato::IsItemVisible(),
            Pato::GetItemRectMin().x, Pato::GetItemRectMin().y,
            Pato::GetItemRectMax().x, Pato::GetItemRectMax().y,
            Pato::GetItemRectSize().x, Pato::GetItemRectSize().y
        );

        static bool embed_all_inside_a_child_window = false;
        Pato::Checkbox("Embed everything inside a child window (for additional testing)", &embed_all_inside_a_child_window);
        if (embed_all_inside_a_child_window)
            Pato::BeginChild("outer_child", ImVec2(0, Pato::GetFontSize() * 20), true);

        // Testing IsWindowFocused() function with its various flags. Note that the flags can be combined.
        Pato::BulletText(
            "IsWindowFocused() = %d\n"
            "IsWindowFocused(_ChildWindows) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowFocused(_RootWindow) = %d\n"
            "IsWindowFocused(_AnyWindow) = %d\n",
            Pato::IsWindowFocused(),
            Pato::IsWindowFocused(PatoFocusedFlags_ChildWindows),
            Pato::IsWindowFocused(PatoFocusedFlags_ChildWindows | PatoFocusedFlags_RootWindow),
            Pato::IsWindowFocused(PatoFocusedFlags_RootWindow),
            Pato::IsWindowFocused(PatoFocusedFlags_AnyWindow));

        // Testing IsWindowHovered() function with its various flags. Note that the flags can be combined.
        Pato::BulletText(
            "IsWindowHovered() = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsWindowHovered(_ChildWindows) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowHovered(_RootWindow) = %d\n"
            "IsWindowHovered(_AnyWindow) = %d\n",
            Pato::IsWindowHovered(),
            Pato::IsWindowHovered(PatoHoveredFlags_AllowWhenBlockedByPopup),
            Pato::IsWindowHovered(PatoHoveredFlags_AllowWhenBlockedByActiveItem),
            Pato::IsWindowHovered(PatoHoveredFlags_ChildWindows),
            Pato::IsWindowHovered(PatoHoveredFlags_ChildWindows | PatoHoveredFlags_RootWindow),
            Pato::IsWindowHovered(PatoHoveredFlags_RootWindow),
            Pato::IsWindowHovered(PatoHoveredFlags_AnyWindow));

        Pato::BeginChild("child", ImVec2(0, 50), true);
        Pato::Text("This is another child window for testing with the _ChildWindows flag.");
        Pato::EndChild();
        if (embed_all_inside_a_child_window)
            Pato::EndChild();

        // Calling IsItemHovered() after begin returns the hovered status of the title bar. 
        // This is useful in particular if you want to create a context menu (with BeginPopupContextItem) associated to the title bar of a window.
        static bool test_window = false;
        Pato::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
        if (test_window)
        {
            Pato::Begin("Title bar Hovered/Active tests", &test_window);
            if (Pato::BeginPopupContextItem()) // <-- This is using IsItemHovered()
            {
                if (Pato::MenuItem("Close")) { test_window = false; }
                Pato::EndPopup();
            }
            Pato::Text(
                "IsItemHovered() after begin = %d (== is title bar hovered)\n"
                "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
                Pato::IsItemHovered(), Pato::IsItemActive());
            Pato::End();
        }

        Pato::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!Pato::CollapsingHeader("Layout"))
        return;

    if (Pato::TreeNode("Child regions"))
    {
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        Pato::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        Pato::Checkbox("Disable Menu", &disable_menu);

        static int line = 50;
        bool goto_line = Pato::Button("Goto");
        Pato::SameLine();
        Pato::PushItemWidth(100);
        goto_line |= Pato::InputInt("##Line", &line, 0, 0, PatoInputTextFlags_EnterReturnsTrue);
        Pato::PopItemWidth();

        // Child 1: no border, enable horizontal scrollbar
        {
            Pato::BeginChild("Child1", ImVec2(Pato::GetWindowContentRegionWidth() * 0.5f, 300), false, PatoWindowFlags_HorizontalScrollbar | (disable_mouse_wheel ? PatoWindowFlags_NoScrollWithMouse : 0));
            for (int i = 0; i < 100; i++)
            {
                Pato::Text("%04d: scrollable region", i);
                if (goto_line && line == i)
                    Pato::SetScrollHereY();
            }
            if (goto_line && line >= 100)
                Pato::SetScrollHereY();
            Pato::EndChild();
        }

        Pato::SameLine();

        // Child 2: rounded border
        {
            Pato::PushStyleVar(PatoStyleVar_ChildRounding, 5.0f);
            Pato::BeginChild("Child2", ImVec2(0, 300), true, (disable_mouse_wheel ? PatoWindowFlags_NoScrollWithMouse : 0) | (disable_menu ? 0 : PatoWindowFlags_MenuBar));
            if (!disable_menu && Pato::BeginMenuBar())
            {
                if (Pato::BeginMenu("Menu"))
                {
                    ShowExampleMenuFile();
                    Pato::EndMenu();
                }
                Pato::EndMenuBar();
            }
            Pato::Columns(2);
            for (int i = 0; i < 100; i++)
            {
                char buf[32];
                sprintf(buf, "%03d", i);
                Pato::Button(buf, ImVec2(-1.0f, 0.0f));
                Pato::NextColumn();
            }
            Pato::EndChild();
            Pato::PopStyleVar();
        }

        Pato::TreePop();
    }

    if (Pato::TreeNode("Widgets Width"))
    {
        static float f = 0.0f;
        Pato::Text("PushItemWidth(100)");
        Pato::SameLine(); ShowHelpMarker("Fixed width.");
        Pato::PushItemWidth(100);
        Pato::DragFloat("float##1", &f);
        Pato::PopItemWidth();

        Pato::Text("PushItemWidth(GetWindowWidth() * 0.5f)");
        Pato::SameLine(); ShowHelpMarker("Half of window width.");
        Pato::PushItemWidth(Pato::GetWindowWidth() * 0.5f);
        Pato::DragFloat("float##2", &f);
        Pato::PopItemWidth();

        Pato::Text("PushItemWidth(GetContentRegionAvailWidth() * 0.5f)");
        Pato::SameLine(); ShowHelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
        Pato::PushItemWidth(Pato::GetContentRegionAvailWidth() * 0.5f);
        Pato::DragFloat("float##3", &f);
        Pato::PopItemWidth();

        Pato::Text("PushItemWidth(-100)");
        Pato::SameLine(); ShowHelpMarker("Align to right edge minus 100");
        Pato::PushItemWidth(-100);
        Pato::DragFloat("float##4", &f);
        Pato::PopItemWidth();

        Pato::Text("PushItemWidth(-1)");
        Pato::SameLine(); ShowHelpMarker("Align to right edge");
        Pato::PushItemWidth(-1);
        Pato::DragFloat("float##5", &f);
        Pato::PopItemWidth();

        Pato::TreePop();
    }

    if (Pato::TreeNode("Basic Horizontal Layout"))
    {
        Pato::TextWrapped("(Use Pato::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        Pato::Text("Two items: Hello"); Pato::SameLine();
        Pato::TextColored(ImVec4(1,1,0,1), "Sailor");

        // Adjust spacing
        Pato::Text("More spacing: Hello"); Pato::SameLine(0, 20);
        Pato::TextColored(ImVec4(1,1,0,1), "Sailor");

        // Button
        Pato::AlignTextToFramePadding();
        Pato::Text("Normal buttons"); Pato::SameLine();
        Pato::Button("Banana"); Pato::SameLine();
        Pato::Button("Apple"); Pato::SameLine();
        Pato::Button("Corniflower");

        // Button
        Pato::Text("Small buttons"); Pato::SameLine();
        Pato::SmallButton("Like this one"); Pato::SameLine();
        Pato::Text("can fit within a text block.");

        // Aligned to arbitrary position. Easy/cheap column.
        Pato::Text("Aligned");
        Pato::SameLine(150); Pato::Text("x=150");
        Pato::SameLine(300); Pato::Text("x=300");
        Pato::Text("Aligned");
        Pato::SameLine(150); Pato::SmallButton("x=150");
        Pato::SameLine(300); Pato::SmallButton("x=300");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        Pato::Checkbox("My", &c1); Pato::SameLine();
        Pato::Checkbox("Tailor", &c2); Pato::SameLine();
        Pato::Checkbox("Is", &c3); Pato::SameLine();
        Pato::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        Pato::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        Pato::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); Pato::SameLine();
        Pato::SliderFloat("X", &f0, 0.0f, 5.0f); Pato::SameLine();
        Pato::SliderFloat("Y", &f1, 0.0f, 5.0f); Pato::SameLine();
        Pato::SliderFloat("Z", &f2, 0.0f, 5.0f);
        Pato::PopItemWidth();

        Pato::PushItemWidth(80);
        Pato::Text("Lists:");
        static int selection[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) Pato::SameLine();
            Pato::PushID(i);
            Pato::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
            Pato::PopID();
            //if (Pato::IsItemHovered()) Pato::SetTooltip("ListBox %d hovered", i);
        }
        Pato::PopItemWidth();

        // Dummy
        ImVec2 button_sz(40, 40);
        Pato::Button("A", button_sz); Pato::SameLine();
        Pato::Dummy(button_sz); Pato::SameLine();
        Pato::Button("B", button_sz);

        // Manually wrapping (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        Pato::Text("Manually wrapping:");
        PatoStyle& style = Pato::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = Pato::GetWindowPos().x + Pato::GetWindowContentRegionMax().x;
        for (int n = 0; n < buttons_count; n++)
        {
            Pato::PushID(n);
            Pato::Button("Box", button_sz);
            float last_button_x2 = Pato::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                Pato::SameLine();
            Pato::PopID();
        }

        Pato::TreePop();
    }

    if (Pato::TreeNode("Tabs"))
    {
        if (Pato::TreeNode("Basic"))
        {
            PatoTabBarFlags tab_bar_flags = PatoTabBarFlags_None;
            if (Pato::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (Pato::BeginTabItem("Avocado"))
                {
                    Pato::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    Pato::EndTabItem();
                }
                if (Pato::BeginTabItem("Broccoli"))
                {
                    Pato::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    Pato::EndTabItem();
                }
                if (Pato::BeginTabItem("Cucumber"))
                {
                    Pato::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    Pato::EndTabItem();
                }
                Pato::EndTabBar();
            }
            Pato::Separator();
            Pato::TreePop();
        }

        if (Pato::TreeNode("Advanced & Close Button"))
        {
            // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static PatoTabBarFlags tab_bar_flags = PatoTabBarFlags_Reorderable;
            Pato::CheckboxFlags("PatoTabBarFlags_Reorderable", (unsigned int*)&tab_bar_flags, PatoTabBarFlags_Reorderable);
            Pato::CheckboxFlags("PatoTabBarFlags_AutoSelectNewTabs", (unsigned int*)&tab_bar_flags, PatoTabBarFlags_AutoSelectNewTabs);
            Pato::CheckboxFlags("PatoTabBarFlags_NoCloseWithMiddleMouseButton", (unsigned int*)&tab_bar_flags, PatoTabBarFlags_NoCloseWithMiddleMouseButton);
            if ((tab_bar_flags & PatoTabBarFlags_FittingPolicyMask_) == 0)
                tab_bar_flags |= PatoTabBarFlags_FittingPolicyDefault_;
            if (Pato::CheckboxFlags("PatoTabBarFlags_FittingPolicyResizeDown", (unsigned int*)&tab_bar_flags, PatoTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(PatoTabBarFlags_FittingPolicyMask_ ^ PatoTabBarFlags_FittingPolicyResizeDown);
            if (Pato::CheckboxFlags("PatoTabBarFlags_FittingPolicyScroll", (unsigned int*)&tab_bar_flags, PatoTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(PatoTabBarFlags_FittingPolicyMask_ ^ PatoTabBarFlags_FittingPolicyScroll);

            // Tab Bar
            const char* names[4] = { "Artichoke", "Beetroot", "Celery", "Daikon" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            {
                if (n > 0) { Pato::SameLine(); }
                Pato::Checkbox(names[n], &opened[n]);
            }

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin(): the underlying bool will be set to false when the tab is closed.
            if (Pato::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && Pato::BeginTabItem(names[n], &opened[n]))
                    {
                        Pato::Text("This is the %s tab!", names[n]);
                        if (n & 1)
                            Pato::Text("I am an odd tab.");
                        Pato::EndTabItem();
                    }
                Pato::EndTabBar();
            }
            Pato::Separator();
            Pato::TreePop();
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Groups"))
    {
        Pato::TextWrapped("(Using Pato::BeginGroup()/EndGroup() to layout items. BeginGroup() basically locks the horizontal position. EndGroup() bundles the whole group so that you can use functions such as IsItemHovered() on it.)");
        Pato::BeginGroup();
        {
            Pato::BeginGroup();
            Pato::Button("AAA");
            Pato::SameLine();
            Pato::Button("BBB");
            Pato::SameLine();
            Pato::BeginGroup();
            Pato::Button("CCC");
            Pato::Button("DDD");
            Pato::EndGroup();
            Pato::SameLine();
            Pato::Button("EEE");
            Pato::EndGroup();
            if (Pato::IsItemHovered())
                Pato::SetTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        ImVec2 size = Pato::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        Pato::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        Pato::Button("ACTION", ImVec2((size.x - Pato::GetStyle().ItemSpacing.x)*0.5f, size.y));
        Pato::SameLine();
        Pato::Button("REACTION", ImVec2((size.x - Pato::GetStyle().ItemSpacing.x)*0.5f, size.y));
        Pato::EndGroup();
        Pato::SameLine();

        Pato::Button("LEVERAGE\nBUZZWORD", size);
        Pato::SameLine();

        if (Pato::ListBoxHeader("List", size))
        {
            Pato::Selectable("Selected", true);
            Pato::Selectable("Not Selected", false);
            Pato::ListBoxFooter();
        }

        Pato::TreePop();
    }

    if (Pato::TreeNode("Text Baseline Alignment"))
    {
        Pato::TextWrapped("(This is testing the vertical alignment that occurs on text to keep it at the same baseline as widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets)");

        Pato::Text("One\nTwo\nThree"); Pato::SameLine();
        Pato::Text("Hello\nWorld"); Pato::SameLine();
        Pato::Text("Banana");

        Pato::Text("Banana"); Pato::SameLine();
        Pato::Text("Hello\nWorld"); Pato::SameLine();
        Pato::Text("One\nTwo\nThree");

        Pato::Button("HOP##1"); Pato::SameLine();
        Pato::Text("Banana"); Pato::SameLine();
        Pato::Text("Hello\nWorld"); Pato::SameLine();
        Pato::Text("Banana");

        Pato::Button("HOP##2"); Pato::SameLine();
        Pato::Text("Hello\nWorld"); Pato::SameLine();
        Pato::Text("Banana");

        Pato::Button("TEST##1"); Pato::SameLine();
        Pato::Text("TEST"); Pato::SameLine();
        Pato::SmallButton("TEST##2");

        Pato::AlignTextToFramePadding(); // If your line starts with text, call this to align it to upcoming widgets.
        Pato::Text("Text aligned to Widget"); Pato::SameLine();
        Pato::Button("Widget##1"); Pato::SameLine();
        Pato::Text("Widget"); Pato::SameLine();
        Pato::SmallButton("Widget##2"); Pato::SameLine();
        Pato::Button("Widget##3");

        // Tree
        const float spacing = Pato::GetStyle().ItemInnerSpacing.x;
        Pato::Button("Button##1");
        Pato::SameLine(0.0f, spacing);
        if (Pato::TreeNode("Node##1")) { for (int i = 0; i < 6; i++) Pato::BulletText("Item %d..", i); Pato::TreePop(); }    // Dummy tree data

        Pato::AlignTextToFramePadding();         // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget. Otherwise you can use SmallButton (smaller fit).
        bool node_open = Pato::TreeNode("Node##2");  // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add child content.
        Pato::SameLine(0.0f, spacing); Pato::Button("Button##2");
        if (node_open) { for (int i = 0; i < 6; i++) Pato::BulletText("Item %d..", i); Pato::TreePop(); }   // Dummy tree data

        // Bullet
        Pato::Button("Button##3");
        Pato::SameLine(0.0f, spacing);
        Pato::BulletText("Bullet text");

        Pato::AlignTextToFramePadding();
        Pato::BulletText("Node");
        Pato::SameLine(0.0f, spacing); Pato::Button("Button##4");

        Pato::TreePop();
    }

    if (Pato::TreeNode("Scrolling"))
    {
        Pato::TextWrapped("(Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given position.)");
        static bool track = true;
        static int track_line = 50, scroll_to_px = 200;
        Pato::Checkbox("Track", &track);
        Pato::PushItemWidth(100);
        Pato::SameLine(130); track |= Pato::DragInt("##line", &track_line, 0.25f, 0, 99, "Line = %d");
        bool scroll_to = Pato::Button("Scroll To Pos");
        Pato::SameLine(130); scroll_to |= Pato::DragInt("##pos_y", &scroll_to_px, 1.00f, 0, 9999, "Y = %d px");
        Pato::PopItemWidth();
        if (scroll_to) track = false;

        for (int i = 0; i < 5; i++)
        {
            if (i > 0) Pato::SameLine();
            Pato::BeginGroup();
            Pato::Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
            Pato::BeginChild(Pato::GetID((void*)(intptr_t)i), ImVec2(Pato::GetWindowWidth() * 0.17f, 200.0f), true);
            if (scroll_to)
                Pato::SetScrollFromPosY(Pato::GetCursorLoopCallbackPos().y + scroll_to_px, i * 0.25f);
            for (int line = 0; line < 100; line++)
            {
                if (track && line == track_line)
                {
                    Pato::TextColored(ImVec4(1,1,0,1), "Line %d", line);
                    Pato::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                }
                else
                {
                    Pato::Text("Line %d", line);
                }
            }
            float scroll_y = Pato::GetScrollY(), scroll_max_y = Pato::GetScrollMaxY();
            Pato::EndChild();
            Pato::Text("%.0f/%0.f", scroll_y, scroll_max_y);
            Pato::EndGroup();
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Horizontal Scrolling"))
    {
        Pato::Bullet(); Pato::TextWrapped("Horizontal scrolling for a window has to be enabled explicitly via the PatoWindowFlags_HorizontalScrollbar flag.");
        Pato::Bullet(); Pato::TextWrapped("You may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
        static int lines = 7;
        Pato::SliderInt("Lines", &lines, 1, 15);
        Pato::PushStyleVar(PatoStyleVar_FrameRounding, 3.0f);
        Pato::PushStyleVar(PatoStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        Pato::BeginChild("scrolling", ImVec2(0, Pato::GetFrameHeightWithSpacing() * 7 + 30), true, PatoWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff (for the sake of this trivial demo we are using basic Button+SameLine. If you want to create your own time line for a real application you may be better off
            // manipulating the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets yourself. You may also want to use the lower-level ImDrawList API)
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) Pato::SameLine();
                Pato::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n%15)) ? "FizzBuzz" : (!(n%3)) ? "Fizz" : (!(n%5)) ? "Buzz" : num_buf;
                float hue = n*0.05f;
                Pato::PushStyleColor(PatoCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                Pato::PushStyleColor(PatoCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                Pato::PushStyleColor(PatoCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                Pato::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                Pato::PopStyleColor(3);
                Pato::PopID();
            }
        }
        float scroll_x = Pato::GetScrollX(), scroll_max_x = Pato::GetScrollMaxX();
        Pato::EndChild();
        Pato::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        Pato::SmallButton("<<"); if (Pato::IsItemActive()) scroll_x_delta = -Pato::GetIO().DeltaTime * 1000.0f; Pato::SameLine();
        Pato::Text("Scroll from code"); Pato::SameLine();
        Pato::SmallButton(">>"); if (Pato::IsItemActive()) scroll_x_delta = +Pato::GetIO().DeltaTime * 1000.0f; Pato::SameLine();
        Pato::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            Pato::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
            Pato::SetScrollX(Pato::GetScrollX() + scroll_x_delta);
            Pato::End();
        }
        Pato::TreePop();
    }

    if (Pato::TreeNode("Clipping"))
    {
        static ImVec2 size(100, 100), offset(50, 20);
        Pato::TextWrapped("On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
        Pato::DragFloat2("size", (float*)&size, 0.5f, 0.0f, 200.0f, "%.0f");
        Pato::TextWrapped("(Click and drag)");
        ImVec2 pos = Pato::GetCursorScreenPos();
        ImVec4 clip_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
        Pato::InvisibleButton("##dummy", size);
        if (Pato::IsItemActive() && Pato::IsMouseDragging()) { offset.x += Pato::GetIO().MouseDelta.x; offset.y += Pato::GetIO().MouseDelta.y; }
        Pato::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(90, 90, 120, 255));
        Pato::GetWindowDrawList()->AddText(Pato::GetFont(), Pato::GetFontSize()*2.0f, ImVec2(pos.x + offset.x, pos.y + offset.y), IM_COL32(255, 255, 255, 255), "Line 1 hello\nLine 2 clip me!", NULL, 0.0f, &clip_rect);
        Pato::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!Pato::CollapsingHeader("Popups & Modal windows"))
        return;

    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by Pato instead of being held by the programmer as we are used to with regular Begin() calls.
    //   User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(PatoHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.

    // Typical use for regular windows:
    //   bool my_tool_is_active = false; if (Pato::Button("Open")) my_tool_is_active = true; [...] if (my_tool_is_active) Begin("My Tool", &my_tool_is_active) { [...] } End();
    // Typical use for popups:
    //   if (Pato::Button("Open")) Pato::OpenPopup("MyPopup"); if (Pato::BeginPopup("MyPopup") { [...] EndPopup(); }

    // With popups we have to go through a library call (here OpenPopup) to manipulate the visibility state.
    // This may be a bit confusing at first but it should quickly make sense. Follow on the examples below. 

    if (Pato::TreeNode("Popups"))
    {
        Pato::TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup
        // (If you want to show the current selection inside the Button itself, you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        if (Pato::Button("Select.."))
            Pato::OpenPopup("my_select_popup");
        Pato::SameLine();
        Pato::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (Pato::BeginPopup("my_select_popup"))
        {
            Pato::Text("Aquarium");
            Pato::Separator();
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (Pato::Selectable(names[i]))
                    selected_fish = i;
            Pato::EndPopup();
        }

        // Showing a menu with toggles
        if (Pato::Button("Toggle.."))
            Pato::OpenPopup("my_toggle_popup");
        if (Pato::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                Pato::MenuItem(names[i], "", &toggles[i]);
            if (Pato::BeginMenu("Sub-menu"))
            {
                Pato::MenuItem("Click me");
                Pato::EndMenu();
            }

            Pato::Separator();
            Pato::Text("Tooltip here");
            if (Pato::IsItemHovered())
                Pato::SetTooltip("I am a tooltip over a popup");

            if (Pato::Button("Stacked Popup"))
                Pato::OpenPopup("another popup");
            if (Pato::BeginPopup("another popup"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    Pato::MenuItem(names[i], "", &toggles[i]);
                if (Pato::BeginMenu("Sub-menu"))
                {
                    Pato::MenuItem("Click me");
                    Pato::EndMenu();
                }
                Pato::EndPopup();
            }
            Pato::EndPopup();
        }

        // Call the more complete ShowExampleMenuFile which we use in various places of this demo
        if (Pato::Button("File Menu.."))
            Pato::OpenPopup("my_file_popup");
        if (Pato::BeginPopup("my_file_popup"))
        {
            ShowExampleMenuFile();
            Pato::EndPopup();
        }

        Pato::TreePop();
    }

    if (Pato::TreeNode("Context menus"))
    {
        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //    if (IsItemHovered() && IsMouseReleased(0))
        //       OpenPopup(id);
        //    return BeginPopup(id);
        // For more advanced uses you may want to replicate and cuztomize this code. This the comments inside BeginPopupContextItem() implementation.
        static float value = 0.5f;
        Pato::Text("Value = %.3f (<-- right-click here)", value);
        if (Pato::BeginPopupContextItem("item context menu"))
        {
            if (Pato::Selectable("Set to zero")) value = 0.0f;
            if (Pato::Selectable("Set to PI")) value = 3.1415f;
            Pato::PushItemWidth(-1);
            Pato::DragFloat("##Value", &value, 0.1f, 0.0f, 0.0f);
            Pato::PopItemWidth();
            Pato::EndPopup();
        }

        // We can also use OpenPopupOnItemClick() which is the same as BeginPopupContextItem() but without the Begin call.
        // So here we will make it that clicking on the text field with the right mouse button (1) will toggle the visibility of the popup above.
        Pato::Text("(You can also right-click me to the same popup as above.)");
        Pato::OpenPopupOnItemClick("item context menu", 1);

        // When used after an item that has an ID (here the Button), we can skip providing an ID to BeginPopupContextItem(). 
        // BeginPopupContextItem() will use the last item ID as the popup ID.
        // In addition here, we want to include your editable label inside the button label. We use the ### operator to override the ID (read FAQ about ID for details)
        static char name[32] = "Label1";
        char buf[64]; sprintf(buf, "Button: %s###Button", name); // ### operator override ID ignoring the preceding label
        Pato::Button(buf);
        if (Pato::BeginPopupContextItem())
        {
            Pato::Text("Edit name:");
            Pato::InputText("##edit", name, IM_ARRAYSIZE(name));
            if (Pato::Button("Close"))
                Pato::CloseCurrentPopup();
            Pato::EndPopup();
        }
        Pato::SameLine(); Pato::Text("(<-- right-click here)");

        Pato::TreePop();
    }

    if (Pato::TreeNode("Modals"))
    {
        Pato::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside the window.");

        if (Pato::Button("Delete.."))
            Pato::OpenPopup("Delete?");

        if (Pato::BeginPopupModal("Delete?", NULL, PatoWindowFlags_AlwaysAutoResize))
        {
            Pato::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            Pato::Separator();

            //static int dummy_i = 0;
            //Pato::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            Pato::PushStyleVar(PatoStyleVar_FramePadding, ImVec2(0, 0));
            Pato::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            Pato::PopStyleVar();

            if (Pato::Button("OK", ImVec2(120, 0))) { Pato::CloseCurrentPopup(); }
            Pato::SetItemDefaultFocus();
            Pato::SameLine();
            if (Pato::Button("Cancel", ImVec2(120, 0))) { Pato::CloseCurrentPopup(); }
            Pato::EndPopup();
        }

        if (Pato::Button("Stacked modals.."))
            Pato::OpenPopup("Stacked 1");
        if (Pato::BeginPopupModal("Stacked 1"))
        {
            Pato::Text("Hello from Stacked The First\nUsing style.Colors[PatoCol_ModalWindowDimBg] behind it.");
            static int item = 1;
            Pato::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
            static float color[4] = { 0.4f,0.7f,0.0f,0.5f };
            Pato::ColorEdit4("color", color);  // This is to test behavior of stacked regular popups over a modal

            if (Pato::Button("Add another modal.."))
                Pato::OpenPopup("Stacked 2");

            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which will close the popup.
            // Note that the visibility state of popups is owned by Pato, so the input value of the bool actually doesn't matter here.
            bool dummy_open = true;
            if (Pato::BeginPopupModal("Stacked 2", &dummy_open))
            {
                Pato::Text("Hello from Stacked The Second!");
                if (Pato::Button("Close"))
                    Pato::CloseCurrentPopup();
                Pato::EndPopup();
            }

            if (Pato::Button("Close"))
                Pato::CloseCurrentPopup();
            Pato::EndPopup();
        }

        Pato::TreePop();
    }

    if (Pato::TreeNode("Menus inside a regular window"))
    {
        Pato::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
        Pato::Separator();
        // NB: As a quirk in this very specific example, we want to differentiate the parent of this menu from the parent of the various popup menus above.
        // To do so we are encloding the items in a PushID()/PopID() block to make them two different menusets. If we don't, opening any popup above and hovering our menu here
        // would open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it, which is the desired behavior for regular menus.
        Pato::PushID("foo");
        Pato::MenuItem("Menu item", "CTRL+M");
        if (Pato::BeginMenu("Menu inside a regular window"))
        {
            ShowExampleMenuFile();
            Pato::EndMenu();
        }
        Pato::PopID();
        Pato::Separator();
        Pato::TreePop();
    }
}

static void ShowDemoWindowColumns()
{
    if (!Pato::CollapsingHeader("Columns"))
        return;

    Pato::PushID("Columns");

    // Basic columns
    if (Pato::TreeNode("Basic"))
    {
        Pato::Text("Without border:");
        Pato::Columns(3, "mycolumns3", false);  // 3-ways, no border
        Pato::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (Pato::Selectable(label)) {}
            //if (Pato::Button(label, ImVec2(-1,0))) {}
            Pato::NextColumn();
        }
        Pato::Columns(1);
        Pato::Separator();

        Pato::Text("With border:");
        Pato::Columns(4, "mycolumns"); // 4-ways, with border
        Pato::Separator();
        Pato::Text("ID"); Pato::NextColumn();
        Pato::Text("Name"); Pato::NextColumn();
        Pato::Text("Path"); Pato::NextColumn();
        Pato::Text("Hovered"); Pato::NextColumn();
        Pato::Separator();
        const char* names[3] = { "One", "Two", "Three" };
        const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (Pato::Selectable(label, selected == i, PatoSelectableFlags_SpanAllColumns))
                selected = i;
            bool hovered = Pato::IsItemHovered();
            Pato::NextColumn();
            Pato::Text(names[i]); Pato::NextColumn();
            Pato::Text(paths[i]); Pato::NextColumn();
            Pato::Text("%d", hovered); Pato::NextColumn();
        }
        Pato::Columns(1);
        Pato::Separator();
        Pato::TreePop();
    }

    // Create multiple items in a same cell before switching to next column
    if (Pato::TreeNode("Mixed items"))
    {
        Pato::Columns(3, "mixed");
        Pato::Separator();

        Pato::Text("Hello");
        Pato::Button("Banana");
        Pato::NextColumn();

        Pato::Text("Pato");
        Pato::Button("Apple");
        static float foo = 1.0f;
        Pato::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        Pato::Text("An extra line here.");
        Pato::NextColumn();

        Pato::Text("Sailor");
        Pato::Button("Corniflower");
        static float bar = 1.0f;
        Pato::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
        Pato::NextColumn();

        if (Pato::CollapsingHeader("Category A")) { Pato::Text("Blah blah blah"); } Pato::NextColumn();
        if (Pato::CollapsingHeader("Category B")) { Pato::Text("Blah blah blah"); } Pato::NextColumn();
        if (Pato::CollapsingHeader("Category C")) { Pato::Text("Blah blah blah"); } Pato::NextColumn();
        Pato::Columns(1);
        Pato::Separator();
        Pato::TreePop();
    }

    // Word wrapping
    if (Pato::TreeNode("Word-wrapping"))
    {
        Pato::Columns(2, "word-wrapping");
        Pato::Separator();
        Pato::TextWrapped("The quick brown fox jumps over the lazy dog.");
        Pato::TextWrapped("Hello Left");
        Pato::NextColumn();
        Pato::TextWrapped("The quick brown fox jumps over the lazy dog.");
        Pato::TextWrapped("Hello Right");
        Pato::Columns(1);
        Pato::Separator();
        Pato::TreePop();
    }

    if (Pato::TreeNode("Borders"))
    {
        // NB: Future columns API should allow automatic horizontal borders.
        static bool h_borders = true;
        static bool v_borders = true;
        Pato::Checkbox("horizontal", &h_borders);
        Pato::SameLine();
        Pato::Checkbox("vertical", &v_borders);
        Pato::Columns(4, NULL, v_borders);
        for (int i = 0; i < 4*3; i++)
        {
            if (h_borders && Pato::GetColumnIndex() == 0)
                Pato::Separator();
            Pato::Text("%c%c%c", 'a'+i, 'a'+i, 'a'+i);
            Pato::Text("Width %.2f\nOffset %.2f", Pato::GetColumnWidth(), Pato::GetColumnOffset());
            Pato::NextColumn();
        }
        Pato::Columns(1);
        if (h_borders)
            Pato::Separator();
        Pato::TreePop();
    }

    // Scrolling columns
    /*
    if (Pato::TreeNode("Vertical Scrolling"))
    {
        Pato::BeginChild("##header", ImVec2(0, Pato::GetTextLineHeightWithSpacing()+Pato::GetStyle().ItemSpacing.y));
        Pato::Columns(3);
        Pato::Text("ID"); Pato::NextColumn();
        Pato::Text("Name"); Pato::NextColumn();
        Pato::Text("Path"); Pato::NextColumn();
        Pato::Columns(1);
        Pato::Separator();
        Pato::EndChild();
        Pato::BeginChild("##scrollingregion", ImVec2(0, 60));
        Pato::Columns(3);
        for (int i = 0; i < 10; i++)
        {
            Pato::Text("%04d", i); Pato::NextColumn();
            Pato::Text("Foobar"); Pato::NextColumn();
            Pato::Text("/path/foobar/%04d/", i); Pato::NextColumn();
        }
        Pato::Columns(1);
        Pato::EndChild();
        Pato::TreePop();
    }
    */

    if (Pato::TreeNode("Horizontal Scrolling"))
    {
        Pato::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
        Pato::BeginChild("##ScrollingRegion", ImVec2(0, Pato::GetFontSize() * 20), false, PatoWindowFlags_HorizontalScrollbar);
        Pato::Columns(10);
        int ITEMS_COUNT = 2000;
        PatoListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
        while (clipper.Step())
        {
            for (int i = clipper.DisplayLoopCallback; i < clipper.DisplayEnd; i++)
                for (int j = 0; j < 10; j++)
                {
                    Pato::Text("Line %d Column %d...", i, j);
                    Pato::NextColumn();
                }
        }
        Pato::Columns(1);
        Pato::EndChild();
        Pato::TreePop();
    }

    bool node_open = Pato::TreeNode("Tree within single cell");
    Pato::SameLine(); ShowHelpMarker("NB: Tree node must be poped before ending the cell. There's no storage of state per-cell.");
    if (node_open)
    {
        Pato::Columns(2, "tree items");
        Pato::Separator();
        if (Pato::TreeNode("Hello")) { Pato::BulletText("Sailor"); Pato::TreePop(); } Pato::NextColumn();
        if (Pato::TreeNode("Bonjour")) { Pato::BulletText("Marin"); Pato::TreePop(); } Pato::NextColumn();
        Pato::Columns(1);
        Pato::Separator();
        Pato::TreePop();
    }
    Pato::PopID();
}

static void ShowDemoWindowMisc()
{
    if (Pato::CollapsingHeader("Filtering"))
    {
        static PatoTextFilter filter;
        Pato::Text("Filter usage:\n"
                    "  \"\"         display all lines\n"
                    "  \"xxx\"      display lines containing \"xxx\"\n"
                    "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                    "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                Pato::BulletText("%s", lines[i]);
    }

    if (Pato::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        PatoIO& io = Pato::GetIO();

        Pato::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        Pato::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        Pato::Text("WantTextInput: %d", io.WantTextInput);
        Pato::Text("WantSetMousePos: %d", io.WantSetMousePos);
        Pato::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        if (Pato::TreeNode("Keyboard, Mouse & Navigation State"))
        {
            if (Pato::IsMousePosValid())
                Pato::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                Pato::Text("Mouse pos: <INVALID>");
            Pato::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            Pato::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f)   { Pato::SameLine(); Pato::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            Pato::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (Pato::IsMouseClicked(i))          { Pato::SameLine(); Pato::Text("b%d", i); }
            Pato::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (Pato::IsMouseDoubleClicked(i)) { Pato::SameLine(); Pato::Text("b%d", i); }
            Pato::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (Pato::IsMouseReleased(i))         { Pato::SameLine(); Pato::Text("b%d", i); }
            Pato::Text("Mouse wheel: %.1f", io.MouseWheel);

            Pato::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f)     { Pato::SameLine(); Pato::Text("%d (%.02f secs)", i, io.KeysDownDuration[i]); }
            Pato::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (Pato::IsKeyPressed(i))             { Pato::SameLine(); Pato::Text("%d", i); }
            Pato::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (Pato::IsKeyReleased(i))            { Pato::SameLine(); Pato::Text("%d", i); }
            Pato::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");

            Pato::Text("NavInputs down:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f)                    { Pato::SameLine(); Pato::Text("[%d] %.2f", i, io.NavInputs[i]); }
            Pato::Text("NavInputs pressed:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f)    { Pato::SameLine(); Pato::Text("[%d]", i); }
            Pato::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f)   { Pato::SameLine(); Pato::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

            Pato::Button("Hovering me sets the\nkeyboard capture flag");
            if (Pato::IsItemHovered())
                Pato::CaptureKeyboardFromApp(true);
            Pato::SameLine();
            Pato::Button("Holding me clears the\nthe keyboard capture flag");
            if (Pato::IsItemActive())
                Pato::CaptureKeyboardFromApp(false);

            Pato::TreePop();
        }

        if (Pato::TreeNode("Tabbing"))
        {
            Pato::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            Pato::InputText("1", buf, IM_ARRAYSIZE(buf));
            Pato::InputText("2", buf, IM_ARRAYSIZE(buf));
            Pato::InputText("3", buf, IM_ARRAYSIZE(buf));
            Pato::PushAllowKeyboardFocus(false);
            Pato::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //Pato::SameLine(); ShowHelperMarker("Use Pato::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
            Pato::PopAllowKeyboardFocus();
            Pato::InputText("5", buf, IM_ARRAYSIZE(buf));
            Pato::TreePop();
        }

        if (Pato::TreeNode("Focus from code"))
        {
            bool focus_1 = Pato::Button("Focus on 1"); Pato::SameLine();
            bool focus_2 = Pato::Button("Focus on 2"); Pato::SameLine();
            bool focus_3 = Pato::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) Pato::SetKeyboardFocusHere();
            Pato::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (Pato::IsItemActive()) has_focus = 1;

            if (focus_2) Pato::SetKeyboardFocusHere();
            Pato::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (Pato::IsItemActive()) has_focus = 2;

            Pato::PushAllowKeyboardFocus(false);
            if (focus_3) Pato::SetKeyboardFocusHere();
            Pato::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (Pato::IsItemActive()) has_focus = 3;
            Pato::PopAllowKeyboardFocus();

            if (has_focus)
                Pato::Text("Item with focus: %d", has_focus);
            else
                Pato::Text("Item with focus: <none>");

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (Pato::Button("Focus on X")) focus_ahead = 0; Pato::SameLine();
            if (Pato::Button("Focus on Y")) focus_ahead = 1; Pato::SameLine();
            if (Pato::Button("Focus on Z")) focus_ahead = 2;
            if (focus_ahead != -1) Pato::SetKeyboardFocusHere(focus_ahead);
            Pato::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            Pato::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            Pato::TreePop();
        }

        if (Pato::TreeNode("Dragging"))
        {
            Pato::TextWrapped("You can use Pato::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
                Pato::Text("IsMouseDragging(%d):\n  w/ default threshold: %d,\n  w/ zero threshold: %d\n  w/ large threshold: %d",
                    button, Pato::IsMouseDragging(button), Pato::IsMouseDragging(button, 0.0f), Pato::IsMouseDragging(button, 20.0f));
            Pato::Button("Drag Me");
            if (Pato::IsItemActive())
            {
                // Draw a line between the button and the mouse cursor
                ImDrawList* draw_list = Pato::GetWindowDrawList();
                draw_list->PushClipRectFullScreen();
                draw_list->AddLine(io.MouseClickedPos[0], io.MousePos, Pato::GetColorU32(PatoCol_Button), 4.0f);
                draw_list->PopClipRect();

                // Drag operations gets "unlocked" when the mouse has moved past a certain threshold (the default threshold is stored in io.MouseDragThreshold)
                // You can request a lower or higher threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta()
                ImVec2 value_raw = Pato::GetMouseDragDelta(0, 0.0f);
                ImVec2 value_with_lock_threshold = Pato::GetMouseDragDelta(0);
                ImVec2 mouse_delta = io.MouseDelta;
                Pato::SameLine(); Pato::Text("Raw (%.1f, %.1f), WithLockThresold (%.1f, %.1f), MouseDelta (%.1f, %.1f)", value_raw.x, value_raw.y, value_with_lock_threshold.x, value_with_lock_threshold.y, mouse_delta.x, mouse_delta.y);
            }
            Pato::TreePop();
        }

        if (Pato::TreeNode("Mouse cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "Move", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == PatoMouseCursor_COUNT);

            Pato::Text("Current mouse cursor = %d: %s", Pato::GetMouseCursor(), mouse_cursors_names[Pato::GetMouseCursor()]);
            Pato::Text("Hover to see mouse cursors:");
            Pato::SameLine(); ShowHelpMarker("Your application can render a different mouse cursor based on what Pato::GetMouseCursor() returns. If software cursor rendering (io.MouseDrawCursor) is set Pato will draw the right cursor for you, otherwise your backend needs to handle it.");
            for (int i = 0; i < PatoMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                Pato::Bullet(); Pato::Selectable(label, false);
                if (Pato::IsItemHovered() || Pato::IsItemFocused())
                    Pato::SetMouseCursor(i);
            }
            Pato::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] About Window / ShowAboutWindow()
// Access from Pato Demo -> Help -> About
//-----------------------------------------------------------------------------

void Pato::ShowAboutWindow(bool* p_open)
{
    if (!Pato::Begin("About Dear Pato", p_open, PatoWindowFlags_AlwaysAutoResize))
    {
        Pato::End();
        return;
    }
    Pato::Text("Dear Pato %s", Pato::GetVersion());
    Pato::Separator();
    Pato::Text("By Omar Cornut and all dear Pato contributors.");
    Pato::Text("Dear Pato is licensed under the MIT License, see LICENSE for more information.");

    static bool show_config_info = false;
    Pato::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        PatoIO& io = Pato::GetIO();
        PatoStyle& style = Pato::GetStyle();

        bool copy_to_clipboard = Pato::Button("Copy to clipboard");
        Pato::BeginChildFrame(Pato::GetID("cfginfos"), ImVec2(0, Pato::GetTextLineHeightWithSpacing() * 18), PatoWindowFlags_NoMove);
        if (copy_to_clipboard)
            Pato::LogToClipboard();

        Pato::Text("Dear Pato %s (%d)", Pato_VERSION, Pato_VERSION_NUM);
        Pato::Separator();
        Pato::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        Pato::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef Pato_DISABLE_OBSOLETE_FUNCTIONS
        Pato::Text("define: Pato_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef Pato_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        Pato::Text("define: Pato_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef Pato_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        Pato::Text("define: Pato_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef Pato_DISABLE_WIN32_FUNCTIONS
        Pato::Text("define: Pato_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef Pato_DISABLE_FORMAT_STRING_FUNCTIONS
        Pato::Text("define: Pato_DISABLE_FORMAT_STRING_FUNCTIONS");
#endif
#ifdef Pato_DISABLE_MATH_FUNCTIONS
        Pato::Text("define: Pato_DISABLE_MATH_FUNCTIONS");
#endif
#ifdef Pato_DISABLE_DEFAULT_ALLOCATORS
        Pato::Text("define: Pato_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef Pato_USE_BGRA_PACKED_COLOR
        Pato::Text("define: Pato_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        Pato::Text("define: _WIN32");
#endif
#ifdef _WIN64
        Pato::Text("define: _WIN64");
#endif
#ifdef __linux__
        Pato::Text("define: __linux__");
#endif
#ifdef __APPLE__
        Pato::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        Pato::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef __MINGW32__
        Pato::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        Pato::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        Pato::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        Pato::Text("define: __clang_version__=%s", __clang_version__);
#endif
        Pato::Separator();
        Pato::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        Pato::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        Pato::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & PatoConfigFlags_NavEnableKeyboard)        Pato::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & PatoConfigFlags_NavEnableGamepad)         Pato::Text(" NavEnableGamepad");
        if (io.ConfigFlags & PatoConfigFlags_NavEnableSetMousePos)     Pato::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & PatoConfigFlags_NavNoCaptureKeyboard)     Pato::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & PatoConfigFlags_NoMouse)                  Pato::Text(" NoMouse");
        if (io.ConfigFlags & PatoConfigFlags_NoMouseCursorChange)      Pato::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         Pato::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   Pato::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              Pato::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            Pato::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       Pato::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        Pato::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & PatoBackendFlags_HasGamepad)             Pato::Text(" HasGamepad");
        if (io.BackendFlags & PatoBackendFlags_HasMouseCursors)        Pato::Text(" HasMouseCursors");
        if (io.BackendFlags & PatoBackendFlags_HasSetMousePos)         Pato::Text(" HasSetMousePos");
        Pato::Separator();
        Pato::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        Pato::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        Pato::Separator();
        Pato::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        Pato::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        Pato::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        Pato::Text("style.FrameRounding: %.2f", style.FrameRounding);
        Pato::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        Pato::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        Pato::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
            Pato::LogFinish();
        Pato::EndChildFrame();
    }
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor / ShowStyleEditor()
//-----------------------------------------------------------------------------

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string. Useful for quick combo boxes where the choices are known locally.
bool Pato::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (Pato::Combo(label, &style_idx, "Classic\0Dark\0Light\0"))
    {
        switch (style_idx)
        {
        case 0: Pato::StyleColorsClassic(); break;
        case 1: Pato::StyleColorsDark(); break;
        case 2: Pato::StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

// Demo helper function to select among loaded fonts.
// Here we use the regular BeginCombo()/EndCombo() api which is more the more flexible one.
void Pato::ShowFontSelector(const char* label)
{
    PatoIO& io = Pato::GetIO();
    ImFont* font_current = Pato::GetFont();
    if (Pato::BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
            if (Pato::Selectable(io.Fonts->Fonts[n]->GetDebugName(), io.Fonts->Fonts[n] == font_current))
                io.FontDefault = io.Fonts->Fonts[n];
        Pato::EndCombo();
    }
    Pato::SameLine();
    ShowHelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and documentation in misc/fonts/ for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

void Pato::ShowStyleEditor(PatoStyle* ref)
{
    // You can pass in a reference PatoStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
    PatoStyle& style = Pato::GetStyle();
    static PatoStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    Pato::PushItemWidth(Pato::GetWindowWidth() * 0.50f);

    if (Pato::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    Pato::ShowFontSelector("Fonts##Selector");

    // Simplified Settings
    if (Pato::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool window_border = (style.WindowBorderSize > 0.0f); if (Pato::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
    Pato::SameLine();
    { bool frame_border = (style.FrameBorderSize > 0.0f); if (Pato::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
    Pato::SameLine();
    { bool popup_border = (style.PopupBorderSize > 0.0f); if (Pato::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }

    // Save/Revert button
    if (Pato::Button("Save Ref"))
        *ref = ref_saved_style = style;
    Pato::SameLine();
    if (Pato::Button("Revert Ref"))
        style = *ref;
    Pato::SameLine();
    ShowHelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. Use \"Export Colors\" below to save them somewhere.");

    Pato::Separator();

    if (Pato::BeginTabBar("##tabs", PatoTabBarFlags_None))
    {
        if (Pato::BeginTabItem("Sizes"))
        {
            Pato::Text("Main");
            Pato::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            Pato::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 16.0f, "%.0f");
            Pato::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            Pato::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            Pato::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            Pato::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            Pato::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            Pato::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            Pato::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            Pato::Text("Borders");
            Pato::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            Pato::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            Pato::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            Pato::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            Pato::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            Pato::Text("Rounding");
            Pato::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 14.0f, "%.0f");
            Pato::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 16.0f, "%.0f");
            Pato::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            Pato::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            Pato::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            Pato::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            Pato::Text("Alignment");
            Pato::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            Pato::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); Pato::SameLine(); ShowHelpMarker("Alignment applies when a button is larger than its text content.");
            Pato::Text("Safe Area Padding"); Pato::SameLine(); ShowHelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            Pato::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            Pato::EndTabItem();
        }

        if (Pato::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (Pato::Button("Export Unsaved"))
            {
                if (output_dest == 0)
                    Pato::LogToClipboard();
                else
                    Pato::LogToTTY();
                Pato::LogText("ImVec4* colors = Pato::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < PatoCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = Pato::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        Pato::LogText("colors[PatoCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                Pato::LogFinish();
            }
            Pato::SameLine(); Pato::PushItemWidth(120); Pato::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); Pato::PopItemWidth();
            Pato::SameLine(); Pato::Checkbox("Only Modified Colors", &output_only_modified);

            static PatoTextFilter filter;
            filter.Draw("Filter colors", Pato::GetFontSize() * 16);

            static PatoColorEditFlags alpha_flags = 0;
            Pato::RadioButton("Opaque", &alpha_flags, 0); Pato::SameLine();
            Pato::RadioButton("Alpha", &alpha_flags, PatoColorEditFlags_AlphaPreview); Pato::SameLine();
            Pato::RadioButton("Both", &alpha_flags, PatoColorEditFlags_AlphaPreviewHalf); Pato::SameLine();
            ShowHelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

            Pato::BeginChild("##colors", ImVec2(0, 0), true, PatoWindowFlags_AlwaysVerticalScrollbar | PatoWindowFlags_AlwaysHorizontalScrollbar | PatoWindowFlags_NavFlattened);
            Pato::PushItemWidth(-160);
            for (int i = 0; i < PatoCol_COUNT; i++)
            {
                const char* name = Pato::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                Pato::PushID(i);
                Pato::ColorEdit4("##color", (float*)&style.Colors[i], PatoColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
                    // Read the FAQ and misc/fonts/README.txt about using icon fonts. It's really easy and super convenient!
                    Pato::SameLine(0.0f, style.ItemInnerSpacing.x); if (Pato::Button("Save")) ref->Colors[i] = style.Colors[i];
                    Pato::SameLine(0.0f, style.ItemInnerSpacing.x); if (Pato::Button("Revert")) style.Colors[i] = ref->Colors[i];
                }
                Pato::SameLine(0.0f, style.ItemInnerSpacing.x);
                Pato::TextUnformatted(name);
                Pato::PopID();
            }
            Pato::PopItemWidth();
            Pato::EndChild();

            Pato::EndTabItem();
        }

        if (Pato::BeginTabItem("Fonts"))
        {
            ImFontAtlas* atlas = Pato::GetIO().Fonts;
            ShowHelpMarker("Read FAQ and misc/fonts/README.txt for details on font loading.");
            Pato::PushItemWidth(120);
            for (int i = 0; i < atlas->Fonts.Size; i++)
            {
                ImFont* font = atlas->Fonts[i];
                Pato::PushID(font);
                bool font_details_opened = Pato::TreeNode(font, "Font %d: \"%s\"\n%.2f px, %d glyphs, %d file(s)", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
                Pato::SameLine(); if (Pato::SmallButton("Set as default")) Pato::GetIO().FontDefault = font;
                if (font_details_opened)
                {
                    Pato::PushFont(font);
                    Pato::Text("The quick brown fox jumps over the lazy dog");
                    Pato::PopFont();
                    Pato::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                    Pato::SameLine(); ShowHelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
                    Pato::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
                    Pato::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                    Pato::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                    Pato::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)sqrtf((float)font->MetricsTotalSurface), (int)sqrtf((float)font->MetricsTotalSurface));
                    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                        if (ImFontConfig* cfg = &font->ConfigData[config_i])
                            Pato::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                    if (Pato::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                    {
                        // Display all glyphs of the fonts in separate pages of 256 characters
                        for (int base = 0; base < 0x10000; base += 256)
                        {
                            int count = 0;
                            for (int n = 0; n < 256; n++)
                                count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
                            if (count > 0 && Pato::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                            {
                                float cell_size = font->FontSize * 1;
                                float cell_spacing = style.ItemSpacing.y;
                                ImVec2 base_pos = Pato::GetCursorScreenPos();
                                ImDrawList* draw_list = Pato::GetWindowDrawList();
                                for (int n = 0; n < 256; n++)
                                {
                                    ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                                    ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                                    const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                                    draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                    if (glyph)
                                        font->RenderChar(draw_list, cell_size, cell_p1, Pato::GetColorU32(PatoCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                    if (glyph && Pato::IsMouseHoveringRect(cell_p1, cell_p2))
                                    {
                                        Pato::BeginTooltip();
                                        Pato::Text("Codepoint: U+%04X", base + n);
                                        Pato::Separator();
                                        Pato::Text("AdvanceX: %.1f", glyph->AdvanceX);
                                        Pato::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                        Pato::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                        Pato::EndTooltip();
                                    }
                                }
                                Pato::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                                Pato::TreePop();
                            }
                        }
                        Pato::TreePop();
                    }
                    Pato::TreePop();
                }
                Pato::PopID();
            }
            if (Pato::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
            {
                Pato::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
                Pato::TreePop();
            }

            static float window_scale = 1.0f;
            if (Pato::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f"))           // scale only this window
                Pato::SetWindowFontScale(window_scale);
            Pato::DragFloat("global scale", &Pato::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f");  // scale everything
            Pato::PopItemWidth();

            Pato::EndTabItem();
        }

        if (Pato::BeginTabItem("Rendering"))
        {
            Pato::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); Pato::SameLine(); ShowHelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
            Pato::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            Pato::PushItemWidth(100);
            Pato::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, "%.2f", 2.0f);
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
            Pato::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            Pato::PopItemWidth();

            Pato::EndTabItem();
        }

        Pato::EndTabBar();
    }

    Pato::PopItemWidth();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------

// Demonstrate creating a fullscreen menu bar and populating it.
static void ShowExampleAppMainMenuBar()
{
    if (Pato::BeginMainMenuBar())
    {
        if (Pato::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            Pato::EndMenu();
        }
        if (Pato::BeginMenu("Edit"))
        {
            if (Pato::MenuItem("Undo", "CTRL+Z")) {}
            if (Pato::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            Pato::Separator();
            if (Pato::MenuItem("Cut", "CTRL+X")) {}
            if (Pato::MenuItem("Copy", "CTRL+C")) {}
            if (Pato::MenuItem("Paste", "CTRL+V")) {}
            Pato::EndMenu();
        }
        Pato::EndMainMenuBar();
    }
}

static void ShowExampleMenuFile()
{
    Pato::MenuItem("(dummy menu)", NULL, false, false);
    if (Pato::MenuItem("New")) {}
    if (Pato::MenuItem("Open", "Ctrl+O")) {}
    if (Pato::BeginMenu("Open Recent"))
    {
        Pato::MenuItem("fish_hat.c");
        Pato::MenuItem("fish_hat.inl");
        Pato::MenuItem("fish_hat.h");
        if (Pato::BeginMenu("More.."))
        {
            Pato::MenuItem("Hello");
            Pato::MenuItem("Sailor");
            if (Pato::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                Pato::EndMenu();
            }
            Pato::EndMenu();
        }
        Pato::EndMenu();
    }
    if (Pato::MenuItem("Save", "Ctrl+S")) {}
    if (Pato::MenuItem("Save As..")) {}
    Pato::Separator();
    if (Pato::BeginMenu("Options"))
    {
        static bool enabled = true;
        Pato::MenuItem("Enabled", "", &enabled);
        Pato::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            Pato::Text("Scrolling Text %d", i);
        Pato::EndChild();
        static float f = 0.5f;
        static int n = 0;
        static bool b = true;
        Pato::SliderFloat("Value", &f, 0.0f, 1.0f);
        Pato::InputFloat("Input", &f, 0.1f);
        Pato::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        Pato::Checkbox("Check", &b);
        Pato::EndMenu();
    }
    if (Pato::BeginMenu("Colors"))
    {
        float sz = Pato::GetTextLineHeight();
        for (int i = 0; i < PatoCol_COUNT; i++)
        {
            const char* name = Pato::GetStyleColorName((PatoCol)i);
            ImVec2 p = Pato::GetCursorScreenPos();
            Pato::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x+sz, p.y+sz), Pato::GetColorU32((PatoCol)i));
            Pato::Dummy(ImVec2(sz, sz));
            Pato::SameLine();
            Pato::MenuItem(name);
        }
        Pato::EndMenu();
    }
    if (Pato::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (Pato::MenuItem("Checked", NULL, true)) {}
    if (Pato::MenuItem("Quit", "Alt+F4")) {}
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
        AddLog("Welcome to Dear Pato!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
    static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
    static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
        ScrollToBottom = true;
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
        ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open)
    {
        Pato::SetNextWindowSize(ImVec2(520,600), PatoCond_FirstUseEver);
        if (!Pato::Begin(title, p_open))
        {
            Pato::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (Pato::BeginPopupContextItem())
        {
            if (Pato::MenuItem("Close Console"))
                *p_open = false;
            Pato::EndPopup();
        }

        Pato::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        Pato::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

        // TODO: display items starting from the bottom

        if (Pato::SmallButton("Add Dummy Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); } Pato::SameLine();
        if (Pato::SmallButton("Add Dummy Error")) { AddLog("[error] something went wrong"); } Pato::SameLine();
        if (Pato::SmallButton("Clear")) { ClearLog(); } Pato::SameLine();
        bool copy_to_clipboard = Pato::SmallButton("Copy"); Pato::SameLine();
        if (Pato::SmallButton("Scroll to bottom")) ScrollToBottom = true;
        //static float t = 0.0f; if (Pato::GetTime() - t > 0.02f) { t = Pato::GetTime(); AddLog("Spam %f", t); }

        Pato::Separator();

        Pato::PushStyleVar(PatoStyleVar_FramePadding, ImVec2(0,0));
        static PatoTextFilter filter;
        filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        Pato::PopStyleVar();
        Pato::Separator();

        const float footer_height_to_reserve = Pato::GetStyle().ItemSpacing.y + Pato::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
        Pato::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, PatoWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
        if (Pato::BeginPopupContextWindow())
        {
            if (Pato::Selectable("Clear")) ClearLog();
            Pato::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use Pato::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
        // You can seek and display only the lines that are visible using the PatoListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
        // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
        //     PatoListClipper clipper(Items.Size);
        //     while (clipper.Step())
        //         for (int i = clipper.DisplayLoopCallback; i < clipper.DisplayEnd; i++)
        // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
        // If your items are of variable size you may want to implement code similar to what PatoListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
        Pato::PushStyleVar(PatoStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        if (copy_to_clipboard)
            Pato::LogToClipboard();
        ImVec4 col_default_text = Pato::GetStyleColorVec4(PatoCol_Text);
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!filter.PassFilter(item))
                continue;
            ImVec4 col = col_default_text;
            if (strstr(item, "[error]")) col = ImColor(1.0f,0.4f,0.4f,1.0f);
            else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f,0.78f,0.58f,1.0f);
            Pato::PushStyleColor(PatoCol_Text, col);
            Pato::TextUnformatted(item);
            Pato::PopStyleColor();
        }
        if (copy_to_clipboard)
            Pato::LogFinish();
        if (ScrollToBottom)
            Pato::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        Pato::PopStyleVar();
        Pato::EndChild();
        Pato::Separator();

        // Command-line
        bool reclaim_focus = false;
        if (Pato::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), PatoInputTextFlags_EnterReturnsTrue|PatoInputTextFlags_CallbackCompletion|PatoInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        Pato::SetItemDefaultFocus();
        if (reclaim_focus)
            Pato::SetKeyboardFocusHere(-1); // Auto focus previous widget

        Pato::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size-1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }
    }

    static int TextEditCallbackStub(PatoInputTextCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(PatoInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionLoopCallback, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case PatoInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (Strnicmp(Commands[i], word_start, (int)(word_end-word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                    data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case PatoInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == PatoKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == PatoKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    PatoTextBuffer     Buf;
    PatoTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset
    bool                ScrollToBottom;

    void    Clear()     { Buf.clear(); LineOffsets.clear(); }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size);
        ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        Pato::SetNextWindowSize(ImVec2(500,400), PatoCond_FirstUseEver);
        if (!Pato::Begin(title, p_open))
        {
            Pato::End();
            return;
        }
        if (Pato::Button("Clear")) Clear();
        Pato::SameLine();
        bool copy = Pato::Button("Copy");
        Pato::SameLine();
        Filter.Draw("Filter", -100.0f);
        Pato::Separator();
        Pato::BeginChild("scrolling", ImVec2(0,0), false, PatoWindowFlags_HorizontalScrollbar);
        if (copy) Pato::LogToClipboard();

        if (Filter.IsActive())
        {
            const char* buf_begin = Buf.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++)
            {
                const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
                if (Filter.PassFilter(line, line_end))
                    Pato::TextUnformatted(line, line_end);
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        }
        else
        {
            Pato::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom)
            Pato::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        Pato::EndChild();
        Pato::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // Demo: add random items (unless Ctrl is held)
    static double last_time = -1.0;
    double time = Pato::GetTime();
    if (time - last_time >= 0.20f && !Pato::GetIO().KeyCtrl)
    {
        const char* random_words[] = { "system", "info", "warning", "error", "fatal", "notice", "log" };
        log.AddLog("[%s] Hello, time is %.1f, frame count is %d\n", random_words[rand() % IM_ARRAYSIZE(random_words)], time, Pato::GetFrameCount());
        last_time = time;
    }

    log.Draw("Example: Log", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
//-----------------------------------------------------------------------------

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
    Pato::SetNextWindowSize(ImVec2(500, 440), PatoCond_FirstUseEver);
    if (Pato::Begin("Example: Simple layout", p_open, PatoWindowFlags_MenuBar))
    {
        if (Pato::BeginMenuBar())
        {
            if (Pato::BeginMenu("File"))
            {
                if (Pato::MenuItem("Close")) *p_open = false;
                Pato::EndMenu();
            }
            Pato::EndMenuBar();
        }

        // left
        static int selected = 0;
        Pato::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < 100; i++)
        {
            char label[128];
            sprintf(label, "MyObject %d", i);
            if (Pato::Selectable(label, selected == i))
                selected = i;
        }
        Pato::EndChild();
        Pato::SameLine();

        // right
        Pato::BeginGroup();
            Pato::BeginChild("item view", ImVec2(0, -Pato::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
                Pato::Text("MyObject: %d", selected);
                Pato::Separator();
                if (Pato::BeginTabBar("##Tabs", PatoTabBarFlags_None))
                {
                    if (Pato::BeginTabItem("Description"))
                    {
                        Pato::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                        Pato::EndTabItem();
                    }
                    if (Pato::BeginTabItem("Details"))
                    {
                        Pato::Text("ID: 0123456789");
                        Pato::EndTabItem();
                    }
                    Pato::EndTabBar();
                }
            Pato::EndChild();
            if (Pato::Button("Revert")) {}
            Pato::SameLine();
            if (Pato::Button("Save")) {}
        Pato::EndGroup();
    }
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------

// Demonstrate create a simple property editor.
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    Pato::SetNextWindowSize(ImVec2(430,450), PatoCond_FirstUseEver);
    if (!Pato::Begin("Example: Property editor", p_open))
    {
        Pato::End();
        return;
    }

    ShowHelpMarker("This example shows how you may implement a property editor using two columns.\nAll objects/fields data are dummies here.\nRemember that in many simple cases, you can use Pato::SameLine(xxx) to position\nyour cursor horizontally instead of using the Columns() API.");

    Pato::PushStyleVar(PatoStyleVar_FramePadding, ImVec2(2,2));
    Pato::Columns(2);
    Pato::Separator();

    struct funcs
    {
        static void ShowDummyObject(const char* prefix, int uid)
        {
            Pato::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
            Pato::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
            bool node_open = Pato::TreeNode("Object", "%s_%u", prefix, uid);
            Pato::NextColumn();
            Pato::AlignTextToFramePadding();
            Pato::Text("my sailor is rich");
            Pato::NextColumn();
            if (node_open)
            {
                static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
                for (int i = 0; i < 8; i++)
                {
                    Pato::PushID(i); // Use field index as identifier.
                    if (i < 2)
                    {
                        ShowDummyObject("Child", 424242);
                    }
                    else
                    {
                        // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                        Pato::AlignTextToFramePadding();
                        Pato::TreeNodeEx("Field", PatoTreeNodeFlags_Leaf | PatoTreeNodeFlags_NoTreePushOnOpen | PatoTreeNodeFlags_Bullet, "Field_%d", i);
                        Pato::NextColumn();
                        Pato::PushItemWidth(-1);
                        if (i >= 5)
                            Pato::InputFloat("##value", &dummy_members[i], 1.0f);
                        else
                            Pato::DragFloat("##value", &dummy_members[i], 0.01f);
                        Pato::PopItemWidth();
                        Pato::NextColumn();
                    }
                    Pato::PopID();
                }
                Pato::TreePop();
            }
            Pato::PopID();
        }
    };

    // Iterate dummy objects with dummy members (all the same data)
    for (int obj_i = 0; obj_i < 3; obj_i++)
        funcs::ShowDummyObject("Object", obj_i);

    Pato::Columns(1);
    Pato::Separator();
    Pato::PopStyleVar();
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
//-----------------------------------------------------------------------------

// Demonstrate/test rendering huge amount of text, and the incidence of clipping.
static void ShowExampleAppLongText(bool* p_open)
{
    Pato::SetNextWindowSize(ImVec2(520,600), PatoCond_FirstUseEver);
    if (!Pato::Begin("Example: Long text display", p_open))
    {
        Pato::End();
        return;
    }

    static int test_type = 0;
    static PatoTextBuffer log;
    static int lines = 0;
    Pato::Text("Printing unusually long amount of text.");
    Pato::Combo("Test type", &test_type, "Single call to TextUnformatted()\0Multiple calls to Text(), clipped manually\0Multiple calls to Text(), not clipped (slow)\0");
    Pato::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (Pato::Button("Clear")) { log.clear(); lines = 0; }
    Pato::SameLine();
    if (Pato::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines+i);
        lines += 1000;
    }
    Pato::BeginChild("Log");
    switch (test_type)
    {
    case 0:
        // Single call to TextUnformatted() with a big buffer
        Pato::TextUnformatted(log.begin(), log.end());
        break;
    case 1:
        {
            // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the PatoListClipper helper.
            Pato::PushStyleVar(PatoStyleVar_ItemSpacing, ImVec2(0,0));
            PatoListClipper clipper(lines);
            while (clipper.Step())
                for (int i = clipper.DisplayLoopCallback; i < clipper.DisplayEnd; i++)
                    Pato::Text("%i The quick brown fox jumps over the lazy dog", i);
            Pato::PopStyleVar();
            break;
        }
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        Pato::PushStyleVar(PatoStyleVar_ItemSpacing, ImVec2(0,0));
        for (int i = 0; i < lines; i++)
            Pato::Text("%i The quick brown fox jumps over the lazy dog", i);
        Pato::PopStyleVar();
        break;
    }
    Pato::EndChild();
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window which gets auto-resized according to its content.
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!Pato::Begin("Example: Auto-resizing window", p_open, PatoWindowFlags_AlwaysAutoResize))
    {
        Pato::End();
        return;
    }

    static int lines = 10;
    Pato::Text("Window will resize every-frame to the size of its content.\nNote that you probably don't want to query the window size to\noutput your content because that would create a feedback loop.");
    Pato::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        Pato::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window with custom resize constraints.
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints // Helper functions to demonstrate programmatic constraints
    {
        static void Square(PatoSizeCallbackData* data) { data->DesiredSize = ImVec2(IM_MAX(data->DesiredSize.x, data->DesiredSize.y), IM_MAX(data->DesiredSize.x, data->DesiredSize.y)); }
        static void Step(PatoSizeCallbackData* data)   { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    static bool auto_resize = false;
    static int type = 0;
    static int display_lines = 10;
    if (type == 0) Pato::SetNextWindowSizeConstraints(ImVec2(-1, 0),    ImVec2(-1, FLT_MAX));      // Vertical only
    if (type == 1) Pato::SetNextWindowSizeConstraints(ImVec2(0, -1),    ImVec2(FLT_MAX, -1));      // Horizontal only
    if (type == 2) Pato::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
    if (type == 3) Pato::SetNextWindowSizeConstraints(ImVec2(400, -1),  ImVec2(500, -1));          // Width 400-500
    if (type == 4) Pato::SetNextWindowSizeConstraints(ImVec2(-1, 400),  ImVec2(-1, 500));          // Height 400-500
    if (type == 5) Pato::SetNextWindowSizeConstraints(ImVec2(0, 0),     ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Square);          // Always Square
    if (type == 6) Pato::SetNextWindowSizeConstraints(ImVec2(0, 0),     ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)100);// Fixed Step

    PatoWindowFlags flags = auto_resize ? PatoWindowFlags_AlwaysAutoResize : 0;
    if (Pato::Begin("Example: Constrained Resize", p_open, flags))
    {
        const char* desc[] =
        {
            "Resize vertical only",
            "Resize horizontal only",
            "Width > 100, Height > 100",
            "Width 400-500",
            "Height 400-500",
            "Custom: Always Square",
            "Custom: Fixed Steps (100)",
        };
        if (Pato::Button("200x200")) { Pato::SetWindowSize(ImVec2(200, 200)); } Pato::SameLine();
        if (Pato::Button("500x500")) { Pato::SetWindowSize(ImVec2(500, 500)); } Pato::SameLine();
        if (Pato::Button("800x200")) { Pato::SetWindowSize(ImVec2(800, 200)); }
        Pato::PushItemWidth(200);
        Pato::Combo("Constraint", &type, desc, IM_ARRAYSIZE(desc));
        Pato::DragInt("Lines", &display_lines, 0.2f, 1, 100);
        Pato::PopItemWidth();
        Pato::Checkbox("Auto-resize", &auto_resize);
        for (int i = 0; i < display_lines; i++)
            Pato::Text("%*sHello, sailor! Making this line long enough for the example.", i * 4, "");
    }
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImVec2 window_pos = ImVec2((corner & 1) ? Pato::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? Pato::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    if (corner != -1)
        Pato::SetNextWindowPos(window_pos, PatoCond_Always, window_pos_pivot);
    Pato::SetNextWindowBgAlpha(0.3f); // Transparent background
    if (Pato::Begin("Example: Simple overlay", p_open, (corner != -1 ? PatoWindowFlags_NoMove : 0) | PatoWindowFlags_NoTitleBar | PatoWindowFlags_NoResize | PatoWindowFlags_AlwaysAutoResize | PatoWindowFlags_NoSavedSettings | PatoWindowFlags_NoFocusOnAppearing | PatoWindowFlags_NoNav))
    {
        Pato::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        Pato::Separator();
        if (Pato::IsMousePosValid())
            Pato::Text("Mouse Position: (%.1f,%.1f)", Pato::GetIO().MousePos.x, Pato::GetIO().MousePos.y);
        else
            Pato::Text("Mouse Position: <invalid>");
        if (Pato::BeginPopupContextWindow())
        {
            if (Pato::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (Pato::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (Pato::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (Pato::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (Pato::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && Pato::MenuItem("Close")) *p_open = false;
            Pato::EndPopup();
        }
    }
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------

// Demonstrate using "##" and "###" in identifiers to manipulate ID generation.
// This apply to all regular items as well. Read FAQ section "How can I have multiple widgets with the same label? Can I have widget without a label? (Yes). A primer on the purpose of labels/IDs." for details.
static void ShowExampleAppWindowTitles(bool*)
{
    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID.

    // Using "##" to display same title but have unique identifier.
    Pato::SetNextWindowPos(ImVec2(100, 100), PatoCond_FirstUseEver);
    Pato::Begin("Same title as another window##1");
    Pato::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    Pato::End();

    Pato::SetNextWindowPos(ImVec2(100, 200), PatoCond_FirstUseEver);
    Pato::Begin("Same title as another window##2");
    Pato::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    Pato::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(Pato::GetTime() / 0.25f) & 3], Pato::GetFrameCount());
    Pato::SetNextWindowPos(ImVec2(100, 300), PatoCond_FirstUseEver);
    Pato::Begin(buf);
    Pato::Text("This window has a changing title.");
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    Pato::SetNextWindowSize(ImVec2(350, 560), PatoCond_FirstUseEver);
    if (!Pato::Begin("Example: Custom rendering", p_open))
    {
        Pato::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
    // Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4.
    // Pato defines overloaded operators but they are internal to Pato.cpp and not exposed outside (to avoid messing with your types)
    // In this example we are not using the maths operators!
    ImDrawList* draw_list = Pato::GetWindowDrawList();

    // Primitives
    Pato::Text("Primitives");
    static float sz = 36.0f;
    static float thickness = 4.0f;
    static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
    Pato::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
    Pato::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
    Pato::ColorEdit3("Color", &col.x);
    {
        const ImVec2 p = Pato::GetCursorScreenPos();
        const ImU32 col32 = ImColor(col);
        float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;
        for (int n = 0; n < 2; n++)
        {
            float curr_thickness = (n == 0) ? 1.0f : thickness;
            draw_list->AddCircle(ImVec2(x+sz*0.5f, y+sz*0.5f), sz*0.5f, col32, 20, curr_thickness); x += sz+spacing;
            draw_list->AddRect(ImVec2(x, y), ImVec2(x+sz, y+sz), col32, 0.0f, ImDrawCornerFlags_All, curr_thickness); x += sz+spacing;
            draw_list->AddRect(ImVec2(x, y), ImVec2(x+sz, y+sz), col32, 10.0f, ImDrawCornerFlags_All, curr_thickness); x += sz+spacing;
            draw_list->AddRect(ImVec2(x, y), ImVec2(x+sz, y+sz), col32, 10.0f, ImDrawCornerFlags_TopLeft|ImDrawCornerFlags_BotRight, curr_thickness); x += sz+spacing;
            draw_list->AddTriangle(ImVec2(x+sz*0.5f, y), ImVec2(x+sz,y+sz-0.5f), ImVec2(x,y+sz-0.5f), col32, curr_thickness); x += sz+spacing;
            draw_list->AddLine(ImVec2(x, y), ImVec2(x+sz, y   ), col32, curr_thickness); x += sz+spacing;   // Horizontal line (note: drawing a filled rectangle will be faster!)
            draw_list->AddLine(ImVec2(x, y), ImVec2(x,    y+sz), col32, curr_thickness); x += spacing;      // Vertical line (note: drawing a filled rectangle will be faster!)
            draw_list->AddLine(ImVec2(x, y), ImVec2(x+sz, y+sz), col32, curr_thickness); x += sz+spacing;   // Diagonal line
            draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x+sz*1.3f,y+sz*0.3f), ImVec2(x+sz-sz*1.3f,y+sz-sz*0.3f), ImVec2(x+sz, y+sz), col32, curr_thickness);
            x = p.x + 4;
            y += sz+spacing;
        }
        draw_list->AddCircleFilled(ImVec2(x+sz*0.5f, y+sz*0.5f), sz*0.5f, col32, 32); x += sz+spacing;
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+sz, y+sz), col32); x += sz+spacing;
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+sz, y+sz), col32, 10.0f); x += sz+spacing;
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+sz, y+sz), col32, 10.0f, ImDrawCornerFlags_TopLeft|ImDrawCornerFlags_BotRight); x += sz+spacing;
        draw_list->AddTriangleFilled(ImVec2(x+sz*0.5f, y), ImVec2(x+sz,y+sz-0.5f), ImVec2(x,y+sz-0.5f), col32); x += sz+spacing;
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+sz, y+thickness), col32); x += sz+spacing;          // Horizontal line (faster than AddLine, but only handle integer thickness)
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+thickness, y+sz), col32); x += spacing+spacing;     // Vertical line (faster than AddLine, but only handle integer thickness)
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x+1, y+1), col32);          x += sz;                  // Pixel (faster than AddLine)
        draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x+sz, y+sz), IM_COL32(0,0,0,255), IM_COL32(255,0,0,255), IM_COL32(255,255,0,255), IM_COL32(0,255,0,255));
        Pato::Dummy(ImVec2((sz+spacing)*8, (sz+spacing)*3));
    }
    Pato::Separator();
    {
        static ImVector<ImVec2> points;
        static bool adding_line = false;
        Pato::Text("Canvas example");
        if (Pato::Button("Clear")) points.clear();
        if (points.Size >= 2) { Pato::SameLine(); if (Pato::Button("Undo")) { points.pop_back(); points.pop_back(); } }
        Pato::Text("Left-click and drag to add lines,\nRight-click to undo");

        // Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered()
        // But you can also draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos().
        // If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max).
        ImVec2 canvas_pos = Pato::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = Pato::GetContentRegionAvail();        // Resize canvas to what's available
        if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
        if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
        draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
        draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

        bool adding_preview = false;
        Pato::InvisibleButton("canvas", canvas_size);
        ImVec2 mouse_pos_in_canvas = ImVec2(Pato::GetIO().MousePos.x - canvas_pos.x, Pato::GetIO().MousePos.y - canvas_pos.y);
        if (adding_line)
        {
            adding_preview = true;
            points.push_back(mouse_pos_in_canvas);
            if (!Pato::IsMouseDown(0))
                adding_line = adding_preview = false;
        }
        if (Pato::IsItemHovered())
        {
            if (!adding_line && Pato::IsMouseClicked(0))
            {
                points.push_back(mouse_pos_in_canvas);
                adding_line = true;
            }
            if (Pato::IsMouseClicked(1) && !points.empty())
            {
                adding_line = adding_preview = false;
                points.pop_back();
                points.pop_back();
            }
        }
        draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);      // clip lines within the canvas (if we resize it, etc.)
        for (int i = 0; i < points.Size - 1; i += 2)
            draw_list->AddLine(ImVec2(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), ImVec2(canvas_pos.x + points[i + 1].x, canvas_pos.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
        draw_list->PopClipRect();
        if (adding_preview)
            points.pop_back();
    }
    Pato::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    const char* Name;           // Document title
    bool        Open;           // Set when the document is open (in this demo, we keep an array of all available documents to simplify the demo)
    bool        OpenPrev;       // Copy of Open from last update.
    bool        Dirty;          // Set when the document has been modified
    bool        WantClose;      // Set when the document 
    ImVec4      Color;          // An arbitrary variable associated to the document

    MyDocument(const char* name, bool open = true, const ImVec4& color = ImVec4(1.0f,1.0f,1.0f,1.0f))
    { 
        Name = name; 
        Open = OpenPrev = open; 
        Dirty = false; 
        WantClose = false; 
        Color = color; 
    }
    void DoOpen()       { Open = true; }
    void DoQueueClose() { WantClose = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave()       { Dirty = false; }

    // Display dummy contents for the Document
    static void DisplayContents(MyDocument* doc)
    {
        Pato::PushID(doc);
        Pato::Text("Document \"%s\"", doc->Name);
        Pato::PushStyleColor(PatoCol_Text, doc->Color);
        Pato::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        Pato::PopStyleColor();
        if (Pato::Button("Modify", ImVec2(100, 0)))
            doc->Dirty = true;
        Pato::SameLine();
        if (Pato::Button("Save", ImVec2(100, 0)))
            doc->DoSave();
        Pato::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        Pato::PopID();
    }

    // Display context menu for the Document
    static void DisplayContextMenu(MyDocument* doc)
    {
        if (!Pato::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (Pato::MenuItem(buf, "CTRL+S", false, doc->Open))
            doc->DoSave();
        if (Pato::MenuItem("Close", "CTRL+W", false, doc->Open))
            doc->DoQueueClose();
        Pato::EndPopup();
    }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument> Documents;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument("Lettuce",             true,  ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("Eggplant",            true,  ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument("Carrot",              true,  ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument("Tomato",              false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("A Rather Long Title", false));
        Documents.push_back(MyDocument("Some Document",       false));
    }
};

// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo, as opposed
// to clicking on the regular tab closing button) and stops being submitted, it will take a frame for the tab bar to notice its absence. 
// During this frame there will be a gap in the tab bar, and if the tab that has disappeared was the selected one, the tab bar 
// will report no selected tab during the frame. This will effectively give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the PatoTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments& app)
{
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev)
            Pato::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    if (!Pato::Begin("Examples: Documents", p_open, PatoWindowFlags_MenuBar))
    {
        Pato::End();
        return;
    }

    // Options
    static bool opt_reorderable = true;
    static PatoTabBarFlags opt_fitting_flags = PatoTabBarFlags_FittingPolicyDefault_;

    // Menu
    if (Pato::BeginMenuBar())
    {
        if (Pato::BeginMenu("File"))
        {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (Pato::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (Pato::MenuItem(doc->Name))
                            doc->DoOpen();
                }
                Pato::EndMenu();
            }
            if (Pato::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                    app.Documents[doc_n].DoQueueClose();
            if (Pato::MenuItem("Exit", "Alt+F4")) {}
            Pato::EndMenu();
        }
        Pato::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0)
            Pato::SameLine();
        Pato::PushID(doc);
        if (Pato::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open)
                doc->DoForceClose();
        Pato::PopID();
    }

    Pato::Separator();

    // Submit Tab Bar and Tabs
    {
        PatoTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? PatoTabBarFlags_Reorderable : 0);
        if (Pato::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                NotifyOfDocumentsClosedElsewhere(app);

            // [DEBUG] Stress tests
            //if ((Pato::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (Pato::GetIO().KeyCtrl) Pato::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                PatoTabItemFlags tab_flags = (doc->Dirty ? PatoTabItemFlags_UnsavedDocument : 0);
                bool visible = Pato::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible)
                {
                    MyDocument::DisplayContents(doc);
                    Pato::EndTabItem();
                }
            }

            Pato::EndTabBar();
        }
    }

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty())
    {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
        {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose)
            {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++)
                close_queue[n]->DoForceClose();
            close_queue.clear();
        }
        else
        {
            if (!Pato::IsPopupOpen("Save?"))
                Pato::OpenPopup("Save?");
            if (Pato::BeginPopupModal("Save?"))
            {
                Pato::Text("Save change to the following items?");
                Pato::PushItemWidth(-1.0f);
                Pato::ListBoxHeader("##", close_queue_unsaved_documents, 6);
                for (int n = 0; n < close_queue.Size; n++)
                    if (close_queue[n]->Dirty)
                        Pato::Text("%s", close_queue[n]->Name);
                Pato::ListBoxFooter();

                if (Pato::Button("Yes", ImVec2(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                    {
                        if (close_queue[n]->Dirty)
                            close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    Pato::CloseCurrentPopup();
                }
                Pato::SameLine();
                if (Pato::Button("No", ImVec2(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        close_queue[n]->DoForceClose();
                    close_queue.clear();
                    Pato::CloseCurrentPopup();
                }
                Pato::SameLine();
                if (Pato::Button("Cancel", ImVec2(80, 0)))
                {
                    close_queue.clear();
                    Pato::CloseCurrentPopup();
                }
                Pato::EndPopup();
            }
        }
    }

    Pato::End();
}

// End of Demo code
#else

void Pato::ShowAboutWindow(bool*) {}
void Pato::ShowDemoWindow(bool*) {}
void Pato::ShowUserGuide() {}
void Pato::ShowStyleEditor(PatoStyle*) {}

#endif
