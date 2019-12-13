#include "GlslShaderLoader.h"
#include "geGL_utils.h"
#include "io/print.h"
#include "shader_literals.h"
#include "time/FPSCounter.h"
#include <Camera.h>
#include <Cell.h>
#include <Model.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <Utilities.h>
#include <chrono>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <iostream>
#include <thread>
#include <Renderers/GridRenderer.h>
#include <Renderers/CellRenderer.h>
#include <Renderers/SimulationCompute.h>

using namespace ge::gl;
using namespace std::string_literals;

auto camera = std::make_shared<Camera>(glm::vec3(1.0, 1.0, 5.0));

bool SDLHandler(const SDL_Event &event) {
  static bool mousePressed = false;
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_UP:
    case SDLK_w:
      camera->ProcessKeyboard(Camera_Movement::FORWARD, 0.1);
      return true;
    case SDLK_DOWN:
    case SDLK_s:
      camera->ProcessKeyboard(Camera_Movement::BACKWARD, 0.1);
      return true;
    case SDLK_LEFT:
    case SDLK_a:
      camera->ProcessKeyboard(Camera_Movement::LEFT, 0.1);
      return true;
    case SDLK_RIGHT:
    case SDLK_d:
      camera->ProcessKeyboard(Camera_Movement::RIGHT, 0.1);
      return true;
    default:
      return false;
    }
  } else if (event.type == SDL_MOUSEBUTTONDOWN and
             event.button.button == SDL_BUTTON_LEFT) {
    mousePressed = true;
    return true;
  } else if (event.type == SDL_MOUSEMOTION and mousePressed) {
    camera->ProcessMouseMovement(-event.motion.xrel, event.motion.yrel);
    return true;
  } else if (event.type == SDL_MOUSEBUTTONUP and
             event.button.button == SDL_BUTTON_LEFT) {
    mousePressed = false;
    return true;
  }
  return false;
}

int main() {
  using namespace ShaderLiterals;
  auto tankSize = glm::uvec3(4, 4, 4);
  const auto fieldOfView = 45.f;
  const auto windowWidth = 640;
  const auto windowHeight = 480;
  const auto nearPlane = 0.1f;
  const auto farPlane = 100.f;
  auto proj = glm::perspective(glm::radians(fieldOfView),
                               static_cast<float>(windowWidth) / windowHeight,
                               nearPlane, farPlane);

  /*Create Window*/
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto window = std::make_shared<sdl2cpp::Window>(windowWidth, windowHeight);
  window->createContext("rendering");
  mainLoop->addWindow("mainWindow", window);
  mainLoop->setEventHandler(SDLHandler);

  /*init OpenGL*/
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  setShaderLocation(SRC_DIR + "/Shaders"s);

  auto cellRenderer = CellRenderer(SRC_DIR + "/Resources/Models/cube.obj"s, proj, tankSize);
  auto gridRenderer = GridRenderer(tankSize, camera, proj);
  auto simulation = SimulationCompute(tankSize, cellRenderer.getIbo(), cellRenderer.getPositionsBuffer());

  simulation.setFluidVolume(15, 1.0);
  simulation.setFluidVolume(16, 1.0);

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  FPSCounter fpsCounter;
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simulation.simulate();

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    cellRenderer.draw(camera->GetViewMatrix(), camera->Position);

    gridRenderer.draw();

    window->swap();
    simulation.swapBuffers();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    fpsCounter.frame();
    print(fpsCounter);
  });

  (*mainLoop)();

  return 0;
}