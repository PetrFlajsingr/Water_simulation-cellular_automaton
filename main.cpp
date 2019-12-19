#include "GlslShaderLoader.h"
#include "io/print.h"
#include "time/FPSCounter.h"
#include <Camera.h>
#include <Renderers/CellRenderer.h>
#include <Renderers/GridRenderer.h>
#include <Renderers/SimulationCompute.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <chrono>
#include <error_handling/exceptions.h>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>
#include <thread>

using namespace std::string_literals;
using namespace std::chrono_literals;
using namespace ge::gl;

auto camera = Camera(glm::vec3(1.0, 1.0, 5.0));
auto simulate = false;

bool SDLHandler(const SDL_Event &event) {
  static bool mousePressed = false;
  if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard) {
    return false;
  }
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_UP:
    case SDLK_w:
      camera.ProcessKeyboard(Camera_Movement::FORWARD, 0.1);
      return true;
    case SDLK_DOWN:
    case SDLK_s:
      camera.ProcessKeyboard(Camera_Movement::BACKWARD, 0.1);
      return true;
    case SDLK_LEFT:
    case SDLK_a:
      camera.ProcessKeyboard(Camera_Movement::LEFT, 0.1);
      return true;
    case SDLK_RIGHT:
    case SDLK_d:
      camera.ProcessKeyboard(Camera_Movement::RIGHT, 0.1);
      return true;
    default:
      return false;
    }
  } else if (event.type == SDL_MOUSEBUTTONDOWN and event.button.button == SDL_BUTTON_LEFT) {
    mousePressed = true;
    return true;
  } else if (event.type == SDL_MOUSEMOTION and mousePressed) {
    camera.ProcessMouseMovement(-event.motion.xrel, event.motion.yrel);
    return true;
  } else if (event.type == SDL_MOUSEBUTTONUP and event.button.button == SDL_BUTTON_LEFT) {
    mousePressed = false;
    return true;
  }
  return false;
}

std::pair<unsigned int, unsigned int> getWindowSize() {
  SDL_DisplayMode DM;
  if (SDL_GetDesktopDisplayMode(0, &DM) != 0)
  {
    throw exc::Error("SDL_GetDesktopDisplayMode failed");
  }
  unsigned int w = DM.w * 0.8;
  unsigned int h = DM.h * 0.8;
  return {w, h};
}

int main() {
  /*Create Window*/
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto tankSize = glm::uvec3(50, 50, 50);
  const auto fieldOfView = 45.f;
  const auto [windowWidth, windowHeight] = getWindowSize();
  const auto nearPlane = 0.1f;
  const auto farPlane = 100.f;
  auto proj = glm::perspective(glm::radians(fieldOfView), static_cast<float>(windowWidth) / windowHeight, nearPlane, farPlane);

  auto window = std::make_shared<sdl2cpp::Window>(windowWidth, windowHeight);
  window->createContext("rendering");
  mainLoop->addWindow("mainWindow", window);
  mainLoop->setEventHandler(SDLHandler);

  /*init OpenGL*/
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  setShaderLocation(SRC_DIR + "/Shaders"s);

  auto cellRenderer = CellRenderer(SRC_DIR + "/Resources/Models/cube.obj"s, proj, tankSize);
  auto gridRenderer = GridRenderer(tankSize, proj);
  auto simulation = SimulationCompute(tankSize, cellRenderer.getIbo(), cellRenderer.getPositionsBuffer());

  simulation.setFluidVolume(15, 1.0);
  simulation.setFluidVolume(14, 1.0);

  using namespace MakeRange;
  for (auto [x, y, z] : range<unsigned int, 3>({5, 5, 5}, {40, 40, 40}, {1, 1, 1})) {
    simulation.setFluidVolume({x, y, z}, 1.0f);
  }

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto io = ImGui::GetIO();
  ImGui_ImplSDL2_InitForOpenGL(window->getWindow(), window->getContext("rendering"));
  ImGui_ImplOpenGL3_Init("#version 450");
  ImGui::StyleColorsDark();

  FPSCounter fpsCounter;
  auto start = std::chrono::system_clock::now();
  float simSpeed = 0.01f;
  bool waterfallEnabled = false;
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window->getWindow());
    ImGui::NewFrame();

    bool *showStatus = nullptr;
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Simulation status", showStatus,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

    ImGui::Text("FPS (avg): %s", std::to_string(static_cast<int>(fpsCounter.average())).c_str());
    if (simulate) {
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
    ImGui::SetWindowPos(ImVec2(window->getWidth() - ImGui::GetWindowWidth(), 0));
    auto previousSize = ImGui::GetWindowSize();
    ImGui::End();

    ImGui::Begin("Simulation", showStatus, ImGuiWindowFlags_AlwaysAutoResize);
    if (!simulate) {
      if (ImGui::Button("Stat Simulation")) {
        simulate = true;
      }
    } else {
      if (ImGui::Button("Pause Simulation")) {
        simulate = false;
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      simulate = false;
      simulation.reset();
      simulation.setFluidVolume(15, 1.0);
      simulation.setFluidVolume(14, 1.0);
    }
    auto waterfallButtonLabel = waterfallEnabled ? "Disable waterfall" : "Enable waterfall";
    if (ImGui::Button(waterfallButtonLabel)) {
      waterfallEnabled = !waterfallEnabled;
    }
    if (waterfallEnabled) {
      for (auto x : range(25, 30, 1)) {
        simulation.setFluidVolume({x, x, x}, 1.0f);
      }
    }
    ImGui::SliderFloat("Simulation speed", &simSpeed, 0.0f, 1.0f);

    ImGui::SetWindowPos(ImVec2(window->getWidth() - ImGui::GetWindowWidth(), previousSize.y));
    previousSize.y += ImGui::GetWindowSize().y;
    ImGui::End();

    ImGui::Begin("Visual", showStatus, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowSize(ImVec2(previousSize.x, ImGui::GetWindowHeight()));
    ImGui::Text("Show:");
    ImGui::SameLine();
    std::array<std::string, 3> items{"None", "Box", "Grid"};
    static std::size_t selected = 1;
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
    ImGui::SetWindowPos(ImVec2(window->getWidth() - ImGui::GetWindowWidth(), previousSize.y));
    ImGui::End();

    if (simulate) {
      auto now = std::chrono::system_clock::now();
      if (simSpeed == 1.f || simSpeed != 0.f && now - start >= 1000ms * (1 - simSpeed)) {
        start = now;
        simulation.simulate();
        simulation.swapBuffers();
      }
    }

    gridRenderer.draw(camera.GetViewMatrix(), DrawType(selected));

    cellRenderer.draw(camera.GetViewMatrix(), camera.Position);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window->swap();
    fpsCounter.frame();
  });

  (*mainLoop)();

  return 0;
}