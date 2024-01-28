#include "CoreLogic.h"
#include <string>

static WNDPROC LastWndProc{};
static twglSwapBuffers LastWglSwapBuffers{};
static void* FuncSwapBuffers{};

bool newWglSwapBuffers(HDC hDC);
LRESULT LogicWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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
    printf("[x] It is not a real Minecraft game process. [CCoreGeneral::GetMinecraftHWND()] \n");
    return NULL;
  };

  HWND curHWND = NULL;
  auto currentPID = CCoreRender::Get()->GetProcessId();
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


HWND CCoreRender::hMinecraft = NULL;
DWORD CCoreRender::ProcessId = NULL;

void CCoreRender::PrivateInit() {
  ProcessId = GetCurrentProcessId();
  printf("[-] Have a good day! :D \n");

  hMinecraft = CCoreGeneral::GetMinecraftHWND();


  if (!hMinecraft) {
    return void(printf("[x] Failed to get game window handle. \n"));
  };

  LastWndProc = (WNDPROC)SetWindowLongPtrW(hMinecraft, GWLP_WNDPROC, (LONG_PTR)LogicWndProc);

  MH_Initialize();

  FuncSwapBuffers = (void*)GetProcAddress((HMODULE)GetModuleHandleW(L"opengl32.dll"), "wglSwapBuffers");

  if (!FuncSwapBuffers) {
    return void(printf("[x] Failed to get wglSwapBuffers(opengl32.dll). \n"));
  }

  MH_CreateHook(FuncSwapBuffers, &newWglSwapBuffers, (LPVOID*)&LastWglSwapBuffers);
  MH_EnableHook(MH_ALL_HOOKS);
};


void CCoreRender::PrivateDestroy() {
  SetWindowLongPtrW(hMinecraft, GWLP_WNDPROC, (LONG_PTR)LastWndProc);
  MH_DisableHook(MH_ALL_HOOKS);
  Render::Destroy();
  MH_RemoveHook(MH_ALL_HOOKS);
}


// Logic Functions

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall LogicWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


  if (Render::IsInit()) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RSHIFT) {
      Render::isVisible = !Render::isVisible;
    };

    if (Render::isVisible && ImGui::IsMousePosValid()) {
      /*
      * 
      * Bullshit, I even cannot solve this problem. Why it doesn't make a effect on window title.
      * 
        RECT rcWnd{};
        RECT rcClientWnd{};
        GetWindowRect(hWnd, &rcWnd);
        GetClientRect(hWnd, &rcClientWnd);
        if (rcWnd.left < LOWORD(lParam) && LOWORD(lParam) < rcWnd.right && rcWnd.top < HIWORD(lParam) && HIWORD(lParam) < rcClientWnd.top) {
          goto CallLastProc;
          return true;
        };
      */

      ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
      return true;
    };
  };

  CallLastProc:
  return CallWindowProcA(LastWndProc, hWnd, uMsg, wParam, lParam);
}

bool __stdcall newWglSwapBuffers(HDC hDC) {
  static HGLRC LastContext{ wglGetCurrentContext() };
  static HGLRC NewContext{};
  static GLint LastViewport[4];

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  static bool init = false;
  if (!init || viewport[2] != LastViewport[2] || viewport[3] != LastViewport[3])
  {
    if (NewContext)
    {
      wglMakeCurrent(hDC, LastContext);
      Render::Shutdown();
      wglDeleteContext(NewContext);
    }

    NewContext = wglCreateContext(hDC);
    wglMakeCurrent(hDC, NewContext);

    glViewport(0, 0, viewport[2], viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    if (!Render::IsInit())
      Render::Init(CCoreRender::GetWindowHandle());
    else
      Render::Update(CCoreRender::GetWindowHandle());

    memcpy(LastViewport, viewport, sizeof(GLint) * 4);

    init = true;
  }
  else {

    wglMakeCurrent(hDC, NewContext);
    Render::Draw();

  }

  wglMakeCurrent(hDC, LastContext);

  return LastWglSwapBuffers(hDC);
}


// Core Render single.


CCoreRender* CCoreRender::p_instance;

CCoreRender::CCoreRender() {
  this->PrivateInit();
}

void CCoreRender::Init() {
  if (p_instance != nullptr)
    return;

  p_instance = new CCoreRender;
}

void CCoreRender::Destroy() {
  if (p_instance == nullptr)
    return;

  p_instance->PrivateDestroy();

  delete p_instance;
  p_instance = nullptr;
}

bool CCoreRender::IsInit() {
  return p_instance != nullptr;
};

CCoreRender* CCoreRender::Get() {
  if (p_instance == nullptr) {
    return nullptr;
  }

  return p_instance;
};

HWND& CCoreRender::GetWindowHandle() {
  return hMinecraft;
};

DWORD CCoreRender::GetProcessId() {
  return ProcessId;
};