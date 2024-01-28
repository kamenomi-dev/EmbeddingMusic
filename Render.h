#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <Windows.h>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

namespace Render {
  static bool isVisible = true;

  void Init(HWND& hWnd);
  void Destroy();
  bool IsInit();

  void Draw();
  void Update(HWND& hWnd);
  void Shutdown();
}

#endif // !RENDER_H
