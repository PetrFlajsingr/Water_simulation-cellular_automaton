//
// Created by Igor Frank on 28.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H

#include <ostream>
#include <CellFlags.h>

class CellInfo {

private:
  float fluidVerticalLefover;
  CellFlags flags;

public:
  void setFlags(CellFlags flags);

  friend std::ostream &operator<<(std::ostream &out, const CellInfo &cell);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H
