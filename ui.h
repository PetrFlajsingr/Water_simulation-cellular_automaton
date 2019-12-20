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

class UI {
public:
UI(sdl2cpp::Window &window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForOpenGL(window.getWindow(), window.getContext("rendering"));
  ImGui_ImplOpenGL3_Init("#version 450");
  ImGui::StyleColorsDark();
}
private:

};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UI_H
