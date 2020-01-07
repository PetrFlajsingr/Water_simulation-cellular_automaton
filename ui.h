//
// Created by petr on 12/19/19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H

#include "Renderers/Simulators/SimulationType.h"
#include "time/FPSCounter.h"
#include <Camera.h>
#include <MapType.h>
#include <SDL2/include/SDL2/SDL_video.h>
#include <SDL2CPP/Window.h>
#include <glm/vec3.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>
#include <utility>

class UI {
public:
  UI(sdl2cpp::Window &window, sdl2cpp::MainLoop &mainLoop);

  void loop();

  void render();

  void setFps(int current, int average);

  void setSimulationRunning(bool running);

  [[nodiscard]] bool isSimulationRunning();
  [[nodiscard]] float simulationSpeed();
  [[nodiscard]] bool isWaterfallEnabled();
  [[nodiscard]] bool isResetPressed();
  [[nodiscard]] unsigned int selectedVisualisation();
  [[nodiscard]] bool isVisualizeVolumes() const;
  [[nodiscard]] int getSimulationSteps() const;
  [[nodiscard]] SimulationType getSelectedMethod() const;
  [[nodiscard]] float getCellSize() const;

  [[nodiscard]] bool isShowVolumes() const;

  void setVolumeText(const std::string &volumeText);
  Camera camera{glm::vec3(1.0, 1.0, 5.0)};

private:
  sdl2cpp::Window &window;
  std::pair<int, int> fps{0, 0};
  bool isSimRunning = false;
  bool waterfallEnabled = false;

  bool showVolumes = true;
  float simSpeed = 1.f;
  float cellSize = 0.5f;
  bool resetPressed = false;
  unsigned int selectedMethod = 1;
  unsigned int selectedMap = 1;

public:
  MapType getSelectedMap() const;
private:
  unsigned int selectedGrid = 1;
  int simulationCycles = 1;
  bool visualizeVolumes = true;
  FPSCounter fpsCounter;

  std::string volumeText = "0.0";

  bool SDLHandler(const SDL_Event &event);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H
