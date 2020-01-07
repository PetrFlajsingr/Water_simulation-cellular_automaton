//
// Created by Igor Frank on 06.01.20.
//

#include "WaterfallMap.h"
#include <various/loc_assert.h>
void WaterfallMap::setup(SimulationCompute &simulation, glm::uvec3 tankSize) {
  loc_assert(glm::all(glm::equal(tankSize, glm::uvec3(32))), "Selected map does not match tank size!");

  simulation.setCells({{23, 48, 0}, {24, 48, 0}, {25, 48, 0}, {26, 48, 0}, {27, 48, 0}}, CellFlags::Solid);
  {
    std::vector<glm::uvec3> result{};
    for (auto z : MakeRange::range(10)) {
      result.emplace_back(glm::uvec3{20, 0, z});
      result.emplace_back(glm::uvec3{20, 1, z});
      result.emplace_back(glm::uvec3{30, 0, z});
      result.emplace_back(glm::uvec3{30, 1, z});
    }
    for (auto x : MakeRange::range(20, 30)) {
      result.emplace_back(glm::uvec3{x, 0, 10});
      result.emplace_back(glm::uvec3{x, 1, 10});
    }
    simulation.setCells(result, CellFlags::Solid);
    simulation.setCells({{23, 49, 0}, {24, 49, 0}, {25, 49, 0}, {26, 49, 0}, {27, 49, 0}}, CellFlags::FluidSource);
  }
}
void WaterfallMap::operator()(SimulationCompute &simulation, BaseMap::Time time) {}

MapType WaterfallMap::getType() { return MapType::Waterfall; }
