//
// Created by Igor Frank on 07.01.20.
//

#include "TearDropMap.h"
#include <chrono>
void TearDropMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 0, 0}, {tankSize.x, 1, tankSize.z}, {1, 1, 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {1, 1, tankSize.z - 1}),
                           CellFlags::Solid);
  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({0, 1, 0}, {tankSize.x, 5, tankSize.z}, {tankSize.x - 1, 1, 1}),
                           CellFlags::Solid);

  simulation.setRangeCells(MakeRange::range<unsigned int, 3>({1, 1, 1}, {tankSize.x - 1, 2, tankSize.z - 1}, {1, 1, 1}),
                           CellFlags::NoFlag, 0.9);

  start = std::chrono::system_clock::now();
}

void TearDropMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {
  using namespace std::chrono_literals;

  auto now = std::chrono::system_clock::now();
  if(now - start >= std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(time))){
    simulation.setRangeCells(MakeRange::range<unsigned int, 3>({15, 31, 15}, {17, 32, 17}, {1, 1, 1}),
                             CellFlags::NoFlag, 1.0);
    start = std::chrono::system_clock::now();
  }
}

MapType TearDropMap::getType() { return MapType::TearDrop; }
