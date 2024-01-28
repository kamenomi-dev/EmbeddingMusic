#pragma once
#ifndef CORELOGIC_H
#define CORELOGIC_H

#include <iostream>

#include <Windows.h>
#include <gl/GL.h>
#include <MinHook.h>

#include "Render.h"

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDC);

class CCoreRender {
  public:
  static HWND& GetWindowHandle();
  static DWORD GetProcessId();

  private:
  CCoreRender();
  void PrivateInit();
  void PrivateDestroy();

public:
  static void Init();
  static void Destroy();
  static bool IsInit();
  static CCoreRender* Get();

private:
  HWND static hMinecraft;
  DWORD static ProcessId;

private:
  static CCoreRender* p_instance;
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
