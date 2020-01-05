//
// Created by Igor Frank on 09.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H

#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

class Cell {
public:
  void setFluidVolume(float fluidVolume);

  float getFluidVolume();

  explicit Cell(float fluidVolume = 0.0, glm::vec4 velocity = glm::vec4(0.0));

private:
  glm::vec4 velocityFluidVolume;
  //float fluidVolume;

  friend std::ostream &operator<<(std::ostream &out, const Cell &cell);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
