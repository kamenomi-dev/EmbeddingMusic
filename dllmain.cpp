#include <thread>
#include <chrono>

#include <Windows.h>

#include "interface/logger.hpp"
#include "CoreLogic.h"

// Thanks for OpenGL-Hk repo (https://github.com/aXXo-dev/OpenGL-Hk/tree/main/src/Dependencies/lib)


void __stdcall ThreadLoop(HINSTANCE hInstance) {

  if (!CCoreGeneral::IsMinecraftProcess()) {
    g_logger << logger_level::LL_ERROR
             << "Sorry, that the target process is not Minecraft game process. "
             << std::endl;
    goto InjectExit;
  };

  g_logger << logger_level::LL_INFO << "Module Loaded! " << std::endl;

  CCoreLogic::Init();

  while (!GetAsyncKeyState(VK_END)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  };

  CCoreLogic::Destroy();

  InjectExit:
  FreeLibrary(hInstance);

};


BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {

  static std::thread thLoop;

  if (dwReason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hInstance);

    // debug
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w+t", stdout);
    g_logger << logger_level::LL_INFO
             << "test" << std::endl;


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