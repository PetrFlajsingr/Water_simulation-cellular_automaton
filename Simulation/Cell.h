//
// Created by Igor Frank on 09.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H

#include <cstdint>

class Cell {
public:
  void setFluidVolume(float fluidVolume);

  void setSolidVolume(float solidVolume);

  void setStable(uint32_t stable);

  explicit Cell(float fluidVolume = 0.0, uint32_t stable = 0, float solidVolume = 0.0);

private:
  float fluidVolume;
  float solidVolume;
  float fluidVerticalLefover;
  uint32_t stable;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
