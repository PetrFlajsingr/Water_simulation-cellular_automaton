//
// Created by Igor Frank on 06.01.20.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASICBOWL_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASICBOWL_H

#include "BaseMap.h"

class BasicBowlMap : public BaseMap {

private:
  void setup(SimulationCompute &simulation, glm::uvec3 tankSize) override;

  void operator()(SimulationCompute &simulation, Time time = 0) override ;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASICBOWL_H
