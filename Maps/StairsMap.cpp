//
// Created by Igor Frank on 07.01.20.
//

#include "StairsMap.h"
#include <various/loc_assert.h>
void StairsMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  loc_assert(glm::all(glm::equal(tankSize, glm::uvec3(32))), "Selected map does not match tank size!");

  for(auto x : MakeRange::downTo(tankSize.x, 16)){
    simulation.setRangeCells(MakeRange::range<unsigned int, 3>({x-16, tankSize.y-x+1, 0}, {x+1-16, tankSize.y-x+2, tankSize.z}, {1, 1, 1}),
                             CellFlags::Solid);
  }

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({15, 0, 0}, {tankSize.x, 1, tankSize.z}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({15, 1, 0}, {tankSize.x, 3, tankSize.z}, {1, 1, tankSize.z - 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({31, 1, 0}, {tankSize.x, 3, tankSize.z}, {1, 1, 1}),
                           CellFlags::Solid);


  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 20, 15}, {1, 21, 17}, {1, 1, 1}),
                           CellFlags::FluidSource);

}
void StairsMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {

}
MapType StairsMap::getType() { return MapType::Stairs; }
