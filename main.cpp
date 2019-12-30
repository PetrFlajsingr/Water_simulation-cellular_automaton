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
#include <thread>

using namespace std::string_literals;
using namespace std::chrono_literals;
using namespace ge::gl;

std::pair<unsigned int, unsigned int> getWindowSize() {
  SDL_DisplayMode DM;
  if (SDL_GetDesktopDisplayMode(0, &DM) != 0) {
    throw exc::Error("SDL_GetDesktopDisplayMode failed");
  }
  unsigned int w = DM.w * 0.8;
  unsigned int h = DM.h * 0.8;
  return {w, h};
}

void initWaterFall(SimulationCompute simulation) {
  simulation.setCells({{23, 48, 0}, {24, 48, 0}, {25, 48, 0}, {26, 48, 0}, {27, 48, 0}}, CellFlags::Cell_Solid);
  {
    std::vector<glm::uvec3> result{};
    for (auto z : MakeRange::range(10)) {
      result.emplace_back(glm::uvec3{20, 0, z});
      result.emplace_back(glm::uvec3{20, 1, z});
      result.emplace_back(glm::uvec3{30, 0, z});
      result.emplace_back(glm::uvec3{30, 1, z});
    }
    for (auto x : MakeRange::range(20, 30)) {
      result.emplace_back(glm::uvec3{x, 0, 10});
      result.emplace_back(glm::uvec3{x, 1, 10});
    }
    simulation.setCells(result, CellFlags::Cell_Solid);
    simulation.setCells({{23, 49, 0}, {24, 49, 0}, {25, 49, 0}, {26, 49, 0}, {27, 49, 0}}, CellFlags::Cell_Source);
  }
}

void initWaterCube(SimulationCompute simulation) {
  {
    using namespace MakeRange;
    std::vector<glm::uvec3> positions;
    std::vector<float> volumes;
    for (auto [x, y, z] : range<unsigned int, 3>({5, 5, 5}, {40, 40, 40}, {1, 1, 1})) {
      positions.emplace_back(glm::vec3{x, y, z});
      volumes.emplace_back(1.0);
    }
    simulation.setCells(positions, CellFlags::Cell_NoFLags, volumes);
  }
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

  /*init OpenGL*/
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  setShaderLocation(SRC_DIR + "/Shaders"s);

  auto cellRenderer = CellRenderer(SRC_DIR + "/Resources/Models/cube.obj"s, proj, tankSize);
  auto gridRenderer = GridRenderer(tankSize, proj);
  auto simulation = SimulationCompute(tankSize);

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPointSize(5);

  UI ui{*window, *mainLoop};

  initWaterFall(simulation);


  simulation.setRangeCells(MakeRange::range<int, 3>({0, 0, 0}, {50, 1, 50}, {1, 1, 1}), CellFlags::Cell_Solid);
  simulation.setRangeCells(MakeRange::range<int, 3>({0, 1, 0}, {50, 5, 50}, {1, 1, 49}), CellFlags::Cell_Solid);
  simulation.setRangeCells(MakeRange::range<int, 3>({0, 1, 0}, {50, 5, 50}, {49, 1, 1}), CellFlags::Cell_Solid);
  simulation.setRangeCells(MakeRange::range<int, 3>({10, 10, 10}, {30, 11, 30}, {1, 1, 1}), CellFlags::Cell_NoFLags, 1.0f);

  auto start = std::chrono::system_clock::now();
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto simSpeed = ui.simulationSpeed();
    if (ui.isSimulationRunning()) {
      auto now = std::chrono::system_clock::now();
      if (simSpeed == 1.f || (simSpeed != 0.f && now - start >= 1000ms * (1 - simSpeed))) {
        start = now;
        for ([[maybe_unused]] auto n : MakeRange::range(ui.getSimulationSteps())) {
          simulation.simulate();
        }
      }
    }
    if (ui.isWaterfallEnabled()) {
      simulation.setCells({{10, 10, 10}, {10, 10, 11}, {10, 10, 12}}, CellFlags::Cell_NoFLags,
                          {1.f, 1.f, 1.f});
    }
    if (ui.isResetPressed()) {
      simulation.reset();
      initWaterFall(simulation);
    }

    gridRenderer.draw(ui.camera.GetViewMatrix(), DrawType(ui.selectedVisualisation()), ui.getCellSize());

    cellRenderer.draw(ui.camera.GetViewMatrix(), ui.camera.Position, simulation.getCellBuffer(), simulation.getInfoCellBuffer(),
                      ui.getCellSize());

    ui.loop();
    ui.render();

    window->swap();
  });

  (*mainLoop)();

  return 0;
}