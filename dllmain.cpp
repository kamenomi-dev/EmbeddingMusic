#include <thread>
#include <chrono>

#include <Windows.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "CoreLogic.h"

// Thanks for OpenGL-Hk repo (https://github.com/aXXo-dev/OpenGL-Hk/tree/main/src/Dependencies/lib)


void __stdcall ThreadLoop(HINSTANCE hInstance) {

  if (!CCoreGeneral::IsMinecraftProcess()) {
    spdlog::error("Sorry, that the target process is not Minecraft game process. ");
    goto InjectExit;
  };

  spdlog::info("Module Loaded! ");

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

    spdlog::flush_on(spdlog::level::debug);

    spdlog::debug("DLL_PROCESS_ATTACH");


    printf(std::string(std::to_string(spdlog::should_log(spdlog::level::info))).c_str());

    thLoop = std::thread([hInstance] { ThreadLoop(hInstance); });
    if (thLoop.joinable()) {
      thLoop.detach();

    };
  };

  if (dwReason == DLL_PROCESS_DETACH) {
    
    spdlog::shutdown();

    FreeConsole();
    fclose(stdout);

  };

  return TRUE;
}