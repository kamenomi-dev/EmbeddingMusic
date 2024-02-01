#include "Render.h"

void CRender::Update(HWND& hWnd) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplWin32_Init(hWnd);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void CRender::Shutdown() {
  isVisible = false;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

// Render Main Bus.

void CRender::Draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  

  if (isVisible) {
	ImGui::Begin("EmbeddingMusic - Thanks, OpenGL-Hk! ", NULL, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Hello, World!");
	ImGui::End();
  };

  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// Singleton stuff

CRender* CRender::p_instance;
std::mutex CRender::mutex;

void CRender::Init(HWND& hWnd) {
  if (p_instance != nullptr) {
	return;
  };

  p_instance = new CRender(hWnd);
}

void CRender::Destroy() {
  if (p_instance == nullptr) {
	return;
  }

  p_instance->Shutdown();

  delete p_instance;
  p_instance = nullptr;
};

CRender* CRender::Get() {
  std::lock_guard<std::mutex> lock(mutex);

  if (p_instance == nullptr)
	return nullptr;

  return p_instance;
}; 

bool CRender::IsInit() {
  return p_instance != nullptr;
}