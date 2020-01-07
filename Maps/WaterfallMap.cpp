//
// Created by Igor Frank on 06.01.20.
//

#include "WaterfallMap.h"
#include <various/loc_assert.h>
void WaterfallMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  loc_assert(glm::all(glm::equal(tankSize, glm::uvec3(32))), "Selected map does not match tank size!");

  simulation.setCells({{14, 30, 0}, {15, 30, 0}, {16, 30, 0}, {17, 30, 0}, {18, 30, 0}}, CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 0, 0}, {tankSize.x, 1, tankSize.z}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {1, 1, tankSize.z - 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {tankSize.x - 1, 1, 1}),
                           CellFlags::Solid);
  simulation.setCells({{14, 31, 0}, {15, 31, 0}, {16, 31, 0}, {17, 31, 0}, {18, 31, 0}}, CellFlags::FluidSource);

}
void WaterfallMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {}

MapType WaterfallMap::getType() { return MapType::Waterfall; }
