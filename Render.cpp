#include "Render.h"

void CRender::Update(HWND& hWnd) {
  ImGui::CreateContext();
  const ImGuiIO& io = ImGui::GetIO();
  resourceList = {
      {"defaultDisplayFont",
       io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 24)},
      {"defaultTitleFont",
       io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeuib.ttf", 24)}
  };
      
  io.Fonts->Build();
  ImGui::StyleColorsDark();
  ImGui_ImplWin32_Init(hWnd);
  ImGui_ImplOpenGL3_Init("#version 330");
}

std::unordered_map<const char*, void*> CRender::GetResource() {
  return this->resourceList;
}

void CRender::Shutdown() {
  Visible(false);
  resourceList.clear();

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

void DrawWindowFrame(CRender* self) {
  const ImGuiIO& io = ImGui::GetIO();
  {
    {
      ImGui::PushFont((ImFont*)self->GetResource()["defaultTitleFont"]);
      ImGui::Text("EmbeddingMusic");
      ImGui::PopFont();
    };
    
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::BeginGroup();
    {
      ImGui::PushItemWidth(78);
      ImGui::PushFont((ImFont*) self->GetResource()["defaultDisplayFont"]);
      ImGui::Text("Combataaaaaaaaaaaaaaaaaa");
      ImGui::PopFont();
      ImGui::PopItemWidth();

      if (ImGui::Button("233")) {
        // todo so
      }
    };
    ImGui::EndGroup();

    {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Open..")) { /* Do stuff */
          }
          if (ImGui::MenuItem("Save")) { /* Do stuff */
          }
          if (ImGui::MenuItem("Close")) {

          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
    }
  }
}



void CRender::Draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  

  if (IsVisible()) {
    ImGui::SetNextWindowSize(ImVec2(800, 600));

    ImGui::Begin("EmbeddingMusic", NULL,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    
    DrawWindowFrame(this);
    Layout::main::Render();

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