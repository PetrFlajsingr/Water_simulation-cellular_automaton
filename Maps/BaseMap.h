//
// Created by Igor Frank on 06.01.20.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASEMAP_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASEMAP_H

#include <Renderers/Simulators/SimulationCompute.h>
#include <MapType.h>
#include <glm/glm.hpp>

class BaseMap {
public:
  using Time = float;
  virtual void setup(SimulationCompute &simulation, glm::uvec3 tankSize) = 0;

  virtual void operator()(SimulationCompute &simulation, Time time) = 0;

  static std::unique_ptr<BaseMap>CreateInstance(MapType type);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASEMAP_H
