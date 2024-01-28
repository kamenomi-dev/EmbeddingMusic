#include "Render.h"

static bool RenderInit{};



void Render::Update(HWND& hWnd) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui::StyleColorsDark();
  ImGui_ImplWin32_Init(hWnd);
  ImGui_ImplOpenGL3_Init();
}

void Render::Shutdown() {
  if (!RenderInit)
	return;

  isVisible = false;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

// Render Main Bus.

void Render::Draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  
  if (isVisible) {
	ImGui::Begin("EmbeddingMusic - Thanks, OpenGL-Hk! ");
	ImGui::Text("Hello, World!");
	ImGui::End();
  };

  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Render::Init(HWND& hWnd) {
  if (RenderInit) {
	return;
  };

  RenderInit = true;
  Update(hWnd);
}

void Render::Destroy() {
  Shutdown();

  RenderInit = false;
};

bool Render::IsInit() {
  return RenderInit;
}