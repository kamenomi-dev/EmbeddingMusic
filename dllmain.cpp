#include <thread>
#include <chrono>

#include <Windows.h>

#include "CoreLogic.h"

// Thanks for OpenGL-Hk repo (https://github.com/aXXo-dev/OpenGL-Hk/tree/main/src/Dependencies/lib)


void __stdcall ThreadLoop(HINSTANCE hInstance) {
  if (!CCoreGeneral::IsMinecraftProcess()) {
    printf("[x] Sorry, that the target process is not Minecraft game process. \n");
    goto InjectExit;
  };

  printf("[-] Module Loaded! \n");

  CCoreRender::Init();

  while (!GetAsyncKeyState(VK_END)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  };

  CCoreRender::Destroy();

  InjectExit:
  FreeLibrary(hInstance);

};


BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {

  static std::thread thLoop;

  if (dwReason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hInstance);

    // debug
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    thLoop = std::thread([hInstance] { ThreadLoop(hInstance); });
    if (thLoop.joinable()) {
      thLoop.detach();
    };
  };

  if (dwReason == DLL_PROCESS_DETACH) {
    FreeConsole();
    fclose(stdout);
  };

  return TRUE;
}