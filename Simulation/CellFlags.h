//
// Created by Igor Frank on 29.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H

enum class CellFlags {
  NoFlag = 0,
  Solid = 1 << 0,
  FluidSource = 1 << 1,
  FluidSink = 1 << 2,
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H
