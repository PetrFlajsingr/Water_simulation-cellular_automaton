//
// Created by Igor Frank on 09.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H

#include <cstdint>
#include <string>
#include <iostream>

enum class CellFlags {
  Cell_NoFLags = 0u,
  Cell_Solid = 1 << 0,
  Cell_Source = 1 << 1,
  Cell_Sink = 1 << 2,
};

class Cell {
public:
  void setFluidVolume(float fluidVolume);

  void setSolidVolume(float solidVolume);

  explicit Cell(float fluidVolume = 0.0, CellFlags flags = CellFlags::Cell_NoFLags, float solidVolume = 0.0);

private:
  float fluidVolume;
  float solidVolume;
  float fluidVerticalLefover;
  CellFlags flags;

public:
  void setFlags(CellFlags flags);

  friend std::ostream &operator<<(std::ostream &out, const Cell &cell);

};


#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
