//
// Created by Igor Frank on 06.01.20.
//

#include "BasicBowlMap.h"
#include <Renderers/Simulators/SimulationCompute.h>
#include <various/loc_assert.h>

void BasicBowlMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  loc_assert(glm::all(glm::equal(tankSize, glm::uvec3(32))), "Selected map does not match tank size!");

  const glm::uvec3 testAreaStart{2, 3, 2};
  const glm::uvec3 testAreaDims{7, 7, 7};
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 0, 0}, {tankSize.x, 1, tankSize.z}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {1, 1, tankSize.z - 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {tankSize.x - 1, 1, 1}),
                           CellFlags::Solid);
  ;
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({testAreaStart.x, testAreaStart.y, testAreaStart.z},
                                                             {testAreaStart.x + testAreaDims.x, testAreaStart.y + testAreaDims.y,
                                                              testAreaStart.z + testAreaDims.z},
                                                             {1, 1, 1}),
                           CellFlags::NoFlag, 1.0f);
}
void BasicBowlMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {
  simulation.setCells({{10, 10, 10}, {10, 10, 11}, {10, 10, 12}}, CellFlags::NoFlag, {.9f, .9f, .9f});
}
