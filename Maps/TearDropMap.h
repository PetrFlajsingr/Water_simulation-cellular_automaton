//
// Created by Igor Frank on 07.01.20.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_TEARDROPMAP_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_TEARDROPMAP_H

#include "BaseMap.h"
class TearDropMap :public BaseMap{
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
public:
  void setup(SimulationCompute &simulation, glm::uvec3 tankSize) override;
  void operator()(SimulationCompute &simulation, Time time) override;
  MapType getType() override;
private:
  TimePoint start;

};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_TEARDROPMAP_H
