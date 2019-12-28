//
// Created by Igor Frank on 28.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H

#include <ostream>
enum class CellFlags {
  Cell_NoFLags = 0u,
  Cell_Solid = 1 << 0,
  Cell_Source = 1 << 1,
  Cell_Sink = 1 << 2,
};

class CellInfo {

private:
  float fluidVerticalLefover;
  CellFlags flags;

public:
  void setFlags(CellFlags flags);

  friend std::ostream &operator<<(std::ostream &out, const CellInfo &cell);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H
