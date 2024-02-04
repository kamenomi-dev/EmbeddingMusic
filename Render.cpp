#include "Render.h"

void CRender::Update(HWND& hWnd) {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplWin32_Init(hWnd);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void CRender::Shutdown() {
  Visible(false);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

bool CRender::IsVisible() {
  return isVisible;
};

void CRender::Visible(bool state) {
  g_logger << logger_level::LL_INFO << "Visible changed, State: " << state
           << std::endl;
  isVisible = state;
};

// Render Main Bus.

void DrawWindowFrame() {

  ImGui::Begin("EmbeddingMusic",
              NULL,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
  {
    ImGui::Text("Well done");
  };
  ImGui::End();
}



void CRender::Draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  

  if (IsVisible()) {
    DrawWindowFrame();
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