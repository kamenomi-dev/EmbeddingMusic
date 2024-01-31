#pragma once
#ifndef CORELOGIC_H
#define CORELOGIC_H

#include <iostream>
#include <memory>
#include <mutex>

#include <Windows.h>
#include <gl/GL.h>
#include <MinHook.h>

#include "Render.h"

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDC);

class CCoreLogic {
  public:
  CCoreLogic(const CCoreLogic()) = delete;
  void operator=(const CCoreLogic&) = delete;

  static bool IsInit();
  static void Init();
  static void Destroy();
  static CCoreLogic* Get();

  private:
  void PrivateInit();
  void PrivateDestroy();

  CCoreLogic() {
    PrivateInit();
  }
  
  static LRESULT __stdcall LogicWndProc(HWND, UINT, WPARAM, LPARAM);
  static bool    __stdcall wglSwapBuffersHook(HDC);

  public:
  HWND hMinecraft{ nullptr };
  DWORD pidMinecraft = NULL;
  void* ptrSwapBuffers{ nullptr };
  twglSwapBuffers lastWglSwapBuffers{ nullptr };

private:
  static CCoreLogic* p_instance;
  static std::mutex mutex;
};

#include <vector>

class CCoreGeneral {
  private:
  CCoreGeneral();
  static void GetWindowByPID(DWORD dwPID, std::vector<HWND>& vecWindows);
  
  public:
  static bool IsMinecraftProcess();
  static HWND GetMinecraftHWND();
};

#endif // ! CORELOGIC_H
