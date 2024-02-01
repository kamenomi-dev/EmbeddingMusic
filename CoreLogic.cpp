#include "CoreLogic.h"
#include <string>

static WNDPROC LastWndProc{ nullptr };

// General functions.

bool CCoreGeneral::IsMinecraftProcess() {
  auto procCmdLine = std::string(GetCommandLineA());

  return (
    procCmdLine.find("mojang") != std::string::npos
    && procCmdLine.find(".minecraft") != std::string::npos
    );
};


void CCoreGeneral::GetWindowByPID(DWORD dwPID, std::vector<HWND>& vecWindows) {
  struct WindowsOfProcess
  {
    std::vector<HWND>* pvecWindows;
    DWORD              dwProcId;

    static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
    {
      DWORD dwProcId;
      GetWindowThreadProcessId(hWnd, &dwProcId);
      if (dwProcId == reinterpret_cast<WindowsOfProcess*>(lParam)->dwProcId)
        reinterpret_cast<WindowsOfProcess*>(lParam)->pvecWindows->push_back(hWnd);
      return TRUE;
    }

    WindowsOfProcess(DWORD dwId, std::vector<HWND>* pvec) : dwProcId(dwId), pvecWindows(pvec) {
      EnumWindows(EnumProc, reinterpret_cast<LPARAM>(this));
    }
  };
  WindowsOfProcess wop(dwPID, &vecWindows);
}


HWND CCoreGeneral::GetMinecraftHWND() {

  if (!IsMinecraftProcess()) {
    spdlog::error("It is not a real Minecraft game process. [CCoreGeneral::GetMinecraftHWND()] ");
    return NULL;
  };

  HWND curHWND = NULL;
  auto currentPID = GetCurrentProcessId();
  std::vector<HWND> vecWnd(0);

  GetWindowByPID(currentPID, vecWnd);
  for (auto item : vecWnd) {
    char* chClassName = new char[255];
    GetClassNameA(item, chClassName, 255);

    if (
      strcmp(chClassName, "LWJGL") == NULL
      || strcmp(chClassName, "GLFW30") == NULL
      ) {

      curHWND = item;
      break;
    }

    delete[] chClassName;
  }

  return curHWND;
}

void CCoreLogic::PrivateInit() {
  pidMinecraft = GetCurrentProcessId();

  spdlog::info("Have a good day! :D ");

  hMinecraft = CCoreGeneral::GetMinecraftHWND();

  if (!hMinecraft) {
    return void(spdlog::error("Failed to get game window handle. "));
  };

  LastWndProc = (WNDPROC)SetWindowLongPtrW(hMinecraft, GWLP_WNDPROC, (LONG_PTR)LogicWndProc);

  MH_Initialize();

  ptrSwapBuffers = (void*)GetProcAddress((HMODULE)GetModuleHandleW(L"opengl32.dll"), "wglSwapBuffers");
  if (!ptrSwapBuffers) {
    return void(spdlog::error("[x] Failed to get wglSwapBuffers(opengl32.dll). \n"));
  }

  MH_CreateHook(ptrSwapBuffers, &wglSwapBuffersHook, (LPVOID*)&lastWglSwapBuffers);
  MH_EnableHook(MH_ALL_HOOKS);
};


void CCoreLogic::PrivateDestroy() {
  SetWindowLongPtrW(hMinecraft, GWLP_WNDPROC, (LONG_PTR)LastWndProc);

  MH_DisableHook(MH_ALL_HOOKS);
  CRender::Get()->Destroy();
  MH_RemoveHook(MH_ALL_HOOKS);
}


// Logic Functions

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall CCoreLogic::LogicWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


  if (CRender::Get()) {
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
      CRender::Get()->isVisible = !CRender::Get()->isVisible;
    };

    if (CRender::Get()->isVisible) {
      if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        if ((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) && (wParam < 256)) {
          return CallWindowProcA(LastWndProc, hWnd, uMsg, wParam, lParam);
        }
        return true;
      }
    };
  };

  return CallWindowProcA(LastWndProc, hWnd, uMsg, wParam, lParam);
}

bool __stdcall CCoreLogic::wglSwapBuffersHook(HDC hDC) {
  static HGLRC LastContext{ wglGetCurrentContext() };
  static HGLRC NewContext{};
  static GLint LastViewport[4];

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  static bool init = false;

  if (!init || viewport[2] != LastViewport[2] || viewport[3] != LastViewport[3]) {
    if (NewContext) {
      wglMakeCurrent(hDC, LastContext);
      CRender::Get()->Shutdown();
      wglDeleteContext(NewContext);
    };

    NewContext = wglCreateContext(hDC);
    wglMakeCurrent(hDC, NewContext);

    glViewport(0, 0, viewport[2], viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glDisable(GL_DEPTH_TEST);


    if (!CRender::IsInit())
      CRender::Init(CCoreLogic::Get()->hMinecraft);
    else
      CRender::Get()->Update(CCoreLogic::Get()->hMinecraft);

    memcpy(LastViewport, viewport, sizeof(GLint) * 4);

    init = true;
  }
  else {

    wglMakeCurrent(hDC, NewContext);
    CRender::Get()->Draw();

  }

  wglMakeCurrent(hDC, LastContext);

  return CCoreLogic::Get()->lastWglSwapBuffers(hDC);
}


// Singleton stuff


CCoreLogic* CCoreLogic::p_instance;
std::mutex CCoreLogic::mutex;

void CCoreLogic::Init() {
  if (p_instance != nullptr)
    return;

  p_instance = new CCoreLogic;
}

void CCoreLogic::Destroy() {
  if (p_instance == nullptr)
    return;

  p_instance->PrivateDestroy();

  delete p_instance;
  p_instance = nullptr;
}

CCoreLogic* CCoreLogic::Get() {
  std::lock_guard<std::mutex> lock(mutex);

  if (p_instance == nullptr) {
    return nullptr;
  }

  return p_instance;
};

bool CCoreLogic::IsInit() {
  return p_instance != nullptr;
};