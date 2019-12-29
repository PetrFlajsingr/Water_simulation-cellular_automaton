//
// Created by Igor Frank on 29.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H

enum class CellFlags {
  Cell_NoFLags = 0u,
  Cell_Solid = 1 << 0,
  Cell_Source = 1 << 1,
  Cell_Sink = 1 << 2,
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H
