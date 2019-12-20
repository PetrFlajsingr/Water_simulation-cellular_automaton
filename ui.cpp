//
// Created by petr on 12/19/19.
//

#include "ui.h"
#include "misc/types/Range.h"

using namespace MakeRange;
UI::UI(sdl2cpp::Window &window) : window(window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForOpenGL(window.getWindow(), window.getContext("rendering"));
  ImGui_ImplOpenGL3_Init("#version 450");
  ImGui::StyleColorsDark();
}

void UI::loop() {
  fpsCounter.frame();
  setFps(fpsCounter.current(), fpsCounter.average());
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window.getWindow());
  ImGui::NewFrame();

  bool *showStatus = nullptr;
  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::Begin("Simulation status", showStatus,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

  ImGui::Text("FPS (avg): %d\nFPS (cur): %d", fps.first, fps.second);
  if (isSimRunning) {
    ImGui::Text("Status: ");
    ImGui::SameLine();
    ImGui::PushStyleColor(0, ImVec4(0.0, 1.0, 0.0, 1.0));
    ImGui::Text("Running");
    ImGui::PopStyleColor(1);
  } else {
    ImGui::Text("Status: ");
    ImGui::SameLine();
    ImGui::PushStyleColor(0, ImVec4(1.0, 0.0, 0.0, 1.0));
    ImGui::Text("Stopped");
    ImGui::PopStyleColor(1);
  }
  ImGui::SetWindowPos(ImVec2(window.getWidth() - ImGui::GetWindowWidth(), 0));
  auto previousSize = ImGui::GetWindowSize();
  ImGui::End();

  ImGui::Begin("Simulation", showStatus, ImGuiWindowFlags_AlwaysAutoResize);
  if (!isSimRunning) {
    if (ImGui::Button("Stat Simulation")) {
      isSimRunning = true;
    }
  } else {
    if (ImGui::Button("Pause Simulation")) {
      isSimRunning = false;
    }
  }
  ImGui::SameLine();
  resetPressed = ImGui::Button("Reset");
  if (resetPressed) {
    isSimRunning = false;
  }
  auto waterfallButtonLabel = waterfallEnabled ? "Disable waterfall" : "Enable waterfall";
  if (ImGui::Button(waterfallButtonLabel)) {
    waterfallEnabled = !waterfallEnabled;
  }

  ImGui::SliderFloat("Simulation speed", &simSpeed, 0.0f, 1.0f);

  ImGui::SetWindowPos(ImVec2(window.getWidth() - ImGui::GetWindowWidth(), previousSize.y));
  previousSize.y += ImGui::GetWindowSize().y;
  ImGui::End();

  ImGui::Begin("Visual", showStatus, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::SetWindowSize(ImVec2(previousSize.x, ImGui::GetWindowHeight()));
  ImGui::Text("Show:");
  ImGui::SameLine();
  std::array<std::string, 3> items{"None", "Box", "Grid"};
  float w = ImGui::CalcItemWidth();
  float spacing = 24.f;
  float button_sz = ImGui::GetFrameHeight();
  ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
  if (ImGui::BeginCombo("", items[selected].c_str())) // The second parameter is the label previewed before opening the combo.
  {
    for (std::size_t n = 0; n < items.size(); n++) {
      bool is_selected = (n == selected); // You can store your selection however you want, outside or inside your objects
      if (ImGui::Selectable(items[n].c_str(), is_selected))
        selected = n;
      if (is_selected)
        ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard
      // navigation support)
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();
  ImGui::SetWindowPos(ImVec2(window.getWidth() - ImGui::GetWindowWidth(), previousSize.y));
  ImGui::End();
}

void UI::render() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void UI::setFps(int current, int average) { fps = {current, average}; }
void UI::setSimulationRunning(bool running) { isSimRunning = running; }
bool UI::isSimulationRunning() { return isSimRunning; }
float UI::simulationSpeed() { return simSpeed; }
bool UI::isWaterfallEnabled() { return waterfallEnabled; }
bool UI::isResetPressed() { return resetPressed; }
unsigned int UI::selectedVisualisation() { return selected; }
