#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <memory>
#include <mutex>

#include <spdlog/spdlog.h>

#include <Windows.h>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

class CRender final {
  public:
  CRender(const CRender&) = delete;
  void operator=(const CRender&) = delete;

  static void Init(HWND& hWnd);
  static void Destroy();
  static CRender* Get();
  static bool IsInit();

  void Draw();
  void Update(HWND& hWnd);
  void Shutdown();

  bool IsVisible();
  void Visible(bool state);

  private:
  CRender(HWND& hWnd)
  {
	Update(hWnd);
  }

  public:

  private:
  bool isVisible = true;

  static CRender* p_instance;
  static std::mutex mutex;
};

#endif // !RENDER_H
