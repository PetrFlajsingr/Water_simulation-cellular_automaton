//
// Created by Igor Frank on 06.01.20.
//

#include "WaterfallMap.h"
#include <various/loc_assert.h>
void WaterfallMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  loc_assert(glm::all(glm::equal(tankSize, glm::uvec3(32))), "Selected map does not match tank size!");

  simulation.setCells({{14, 30, 0}, {15, 30, 0}, {16, 30, 0}, {17, 30, 0}, {18, 30, 0}}, CellFlags::Solid);
  {
    std::vector<glm::uvec3> result{};
    for (auto z : MakeRange::range(10)) {
      result.emplace_back(glm::uvec3{5, 0, z});
      result.emplace_back(glm::uvec3{5, 1, z});
      result.emplace_back(glm::uvec3{25, 0, z});
      result.emplace_back(glm::uvec3{25, 1, z});
    }
    for (auto x : MakeRange::range(5, 25)) {
      result.emplace_back(glm::uvec3{x, 0, 10});
      result.emplace_back(glm::uvec3{x, 1, 10});
    }
    simulation.setCells(result, CellFlags::Solid);
    simulation.setCells({{14, 31, 0}, {15, 31, 0}, {16, 31, 0}, {17, 31, 0}, {18, 31, 0}}, CellFlags::FluidSource);
  }
}
void WaterfallMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {}

MapType WaterfallMap::getType() { return MapType::Waterfall; }
