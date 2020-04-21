// dear Pato: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID in Pato.cpp.

// You can copy and use unmodified Pato_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear Pato, read examples/README.txt and read the documentation at the top of Pato.cpp.
// https://github.com/ocornut/Pato

#pragma once

struct IDirect3DDevice9;

Pato_IMPL_API bool     Pato_ImplDX9_Init(IDirect3DDevice9* device);
Pato_IMPL_API void     Pato_ImplDX9_Shutdown();
Pato_IMPL_API void     Pato_ImplDX9_NewFrame();
Pato_IMPL_API void     Pato_ImplDX9_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing Pato state.
Pato_IMPL_API void     Pato_ImplDX9_InvalidateDeviceObjects();
Pato_IMPL_API bool     Pato_ImplDX9_CreateDeviceObjects();
