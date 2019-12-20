#include "GlslShaderLoader.h"
#include "io/print.h"
#include "ui.h"
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
#include <imgui/imgui_impl_sdl.h>
#include <thread>

using namespace std::string_literals;
using namespace std::chrono_literals;
using namespace ge::gl;

auto camera = Camera(glm::vec3(1.0, 1.0, 5.0));

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
  if (SDL_GetDesktopDisplayMode(0, &DM) != 0) {
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

  using namespace MakeRange;
  for (auto [x, y, z] : range<unsigned int, 3>({5, 5, 5}, {40, 40, 40}, {1, 1, 1})) {
    simulation.setFluidVolume({x, y, z}, 1.0f);
  }

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  UI ui{*window};


  auto start = std::chrono::system_clock::now();
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto simSpeed = ui.simulationSpeed();
    if (ui.isSimulationRunning()) {
      auto now = std::chrono::system_clock::now();
      if (simSpeed == 1.f || (simSpeed != 0.f && now - start >= 1000ms * (1 - simSpeed))) {
        start = now;
        simulation.simulate();
        simulation.swapBuffers();
      }
    }
    if (ui.isWaterfallEnabled()) {
      simulation.setFluidVolume({20, 20, 20}, 1.0);
      simulation.setFluidVolume({21, 20, 20}, 1.0);
      simulation.setFluidVolume({20, 20, 21}, 1.0);
      simulation.setFluidVolume({21, 20, 21}, 1.0);
    }
    if (ui.isResetPressed()) {
      simulation.reset();
    }

    gridRenderer.draw(camera.GetViewMatrix(), DrawType(ui.selectedVisualisation()));

    cellRenderer.draw(camera.GetViewMatrix(), camera.Position);

    ui.loop();
    ui.render();

    window->swap();
  });

  (*mainLoop)();

  return 0;
}