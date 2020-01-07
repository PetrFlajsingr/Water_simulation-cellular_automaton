//
// Created by Igor Frank on 07.01.20.
//

#include "OverflowMap.h"
#include <various/loc_assert.h>
void OverflowMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  loc_assert(glm::all(glm::equal(tankSize, glm::uvec3(32))), "Selected map does not match tank size!");

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({12, 19, 12}, {20, 20, 20}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({12, 20, 12}, {20, 23, 20}, {1, 1, 7}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({12, 20, 12}, {20, 23, 20}, {7, 1, 1}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({8, 9, 8}, {24, 10, 24}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({8, 10, 8}, {24, 13, 24}, {1, 1, 15}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({8, 10, 8}, {24, 13, 24}, {15, 1, 1}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 0, 0}, {tankSize.x, 1, tankSize.z}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {1, 1, tankSize.z - 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {tankSize.x - 1, 1, 1}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({15, 31, 15}, {17, 32, 17}, {1, 1, 1}),
                           CellFlags::FluidSource);
}

void OverflowMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {

}

MapType OverflowMap::getType() { return MapType::Overflow; }
