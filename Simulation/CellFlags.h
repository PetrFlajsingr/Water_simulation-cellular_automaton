//
// Created by Igor Frank on 29.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H

enum class CellFlags : unsigned int {
  NoFlag = 0u,
  Solid = 1u << 0u,
  FluidSource = 1u << 1u,
  FluidSink = 1u << 2u,
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLFLAGS_H
