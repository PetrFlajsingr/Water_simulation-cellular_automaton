//
// Created by petr on 12/19/19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H

#include <SDL2/include/SDL2/SDL_video.h>
#include <SDL2CPP/Window.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>
#include "time/FPSCounter.h"

class UI {
public:
  explicit UI(sdl2cpp::Window &window);

  void loop();

  void render();

  void setFps(int current, int average);

  void setSimulationRunning(bool running);

  [[nodiscard]] bool isSimulationRunning();
  [[nodiscard]] float simulationSpeed();
  [[nodiscard]] bool isWaterfallEnabled();
  [[nodiscard]] bool isResetPressed();
  [[nodiscard]] unsigned int selectedVisualisation();

private:
  sdl2cpp::Window &window;
  std::pair<int, int> fps = {0, 0};
  bool isSimRunning = false;
  bool waterfallEnabled = false;
  float simSpeed = 0.f;
  bool resetPressed = false;
  unsigned int selected = 1;

  FPSCounter fpsCounter;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H
