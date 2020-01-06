//
// Created by Igor Frank on 28.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H

#include <CellFlags.h>
#include <ostream>

class CellInfo {
public:
  void setFlags(CellFlags flags);

  friend std::ostream &operator<<(std::ostream &out, const CellInfo &cell);

private:
  float fluidVerticalLefover;

  CellFlags flags;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFO_H
