#include "GlslShaderLoader.h"
#include "io/print.h"
#include "ui.h"
#include <BaseMap.h>
#include <Camera.h>
#include <MapType.h>
#include <Renderers/CellRenderer.h>
#include <Renderers/GridRenderer.h>
#include <Renderers/Simulators/SimulationCompute.h>
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
  const auto w = static_cast<unsigned int>(DM.w * 0.8);
  const auto h = static_cast<unsigned int>(DM.h * 0.8);
  return {w, h};
}

int main() {
  /*Create Window*/
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto tankSize = glm::uvec3(32, 32, 32);
  const auto fieldOfView = 45.f;
  const auto [windowWidth, windowHeight] = getWindowSize();
  const auto nearPlane = 0.1f;
  const auto farPlane = 100.f;
  const auto proj =
      glm::perspective(glm::radians(fieldOfView), static_cast<float>(windowWidth) / windowHeight, nearPlane, farPlane);

  auto window = std::make_shared<sdl2cpp::Window>(windowWidth, windowHeight);
  window->createContext("rendering");
  mainLoop->addWindow("mainWindow", window);

  /*init OpenGL*/
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  setShaderLocation(SRC_DIR + "/Shaders"s);

  UI ui{*window, *mainLoop};
  auto cellRenderer = CellRenderer(SRC_DIR + "/Resources/Models/cube.obj"s, proj, tankSize);
  auto gridRenderer = GridRenderer(tankSize, proj);
  auto simulationType = ui.getSelectedMethod();
  auto simulation = SimulationCompute::CreateInstance(ui.getSelectedMethod(), tankSize);

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPointSize(5);

  auto map = BaseMap::CreateInstance(MapType::Waterfall);
  map->setup(*simulation, tankSize);

  auto start = std::chrono::system_clock::now();
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT(hicpp-signed-bitwise)

    if(ui.getSelectedMap() != map->getType()){
      simulation->reset();
      map = BaseMap::CreateInstance(ui.getSelectedMap());
      map->setup(*simulation, tankSize);
    }

    if (simulationType != ui.getSelectedMethod()) {
      simulationType = ui.getSelectedMethod();
      simulation = SimulationCompute::CreateInstance(simulationType, tankSize);
      map->setup(*simulation, tankSize);
    }

    const auto simSpeed = ui.simulationSpeed();
    if (ui.isSimulationRunning()) {
      auto now = std::chrono::system_clock::now();
      if (simSpeed == 1.f || (simSpeed != 0.f && now - start >= 1000ms * (1 - simSpeed))) {
        start = now;
        for ([[maybe_unused]] auto n : MakeRange::range(ui.getSimulationSteps())) {
          simulation->simulate();
        }
      }
    }
    if (ui.isResetPressed()) {
      simulation->reset();
      map->setup(*simulation, tankSize);
    }
    if (ui.isShowVolumes()) {
      ui.setVolumeText(std::to_string(simulation->getFluidVolume()));
    }

    if (ui.isWaterfallEnabled()) {
      (*map)(*simulation, 0);
    }

    gridRenderer.draw(ui.camera.GetViewMatrix(), DrawType(ui.selectedVisualisation()), ui.getCellSize());

    switch (simulationType) {
    case SimulationType::Basic:
      cellRenderer.drawBasic(ui.camera.GetViewMatrix(), ui.camera.Position, simulation->getCellBuffer(),
                             simulation->getInfoCellBuffer(), ui.isVisualizeVolumes(), ui.getCellSize());
      break;
    case SimulationType::Advanced:
      cellRenderer.drawAdvanced(ui.camera.GetViewMatrix(), ui.camera.Position, simulation->getCellBuffer(),
                                simulation->getInfoCellBuffer(), ui.isVisualizeVolumes(), ui.getCellSize());
      break;
    }

    ui.loop();
    ui.render();

    window->swap();
  });

  (*mainLoop)();

  return 0;
}