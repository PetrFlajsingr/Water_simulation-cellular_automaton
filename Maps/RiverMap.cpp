//
// Created by Igor Frank on 07.01.20.
//

#include "RiverMap.h"
void RiverMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 3, 8}, {8, 4, 25}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 3, 8}, {8, 8, 25}, {1, 1, 16}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({8, 2, 8}, {20, 3, 25}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({8, 2, 8}, {20, 7, 25}, {1, 1, 16}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({20, 1, 8}, {32, 2, 25}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({20, 1, 8}, {32, 6, 25}, {1, 1, 16}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 4, 9}, {1, 5, 24}, {1, 1, 1}),
                           CellFlags::FluidSource);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({31, 2, 9}, {32, 3, 24}, {1, 1, 1}),
                           CellFlags::FluidSink);
}
void RiverMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {}
MapType RiverMap::getType() { return MapType::River; }
