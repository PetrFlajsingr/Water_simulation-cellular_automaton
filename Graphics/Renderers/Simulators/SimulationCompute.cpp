//
// Created by Igor Frank on 02.01.20.
//

#include "SimulationCompute.h"
#include "AdvancedSimulationCompute.h"
#include "BasicSimulationCompute.h"
#include <glm/gtx/component_wise.hpp>

const SimulationCompute::BufferPtr &SimulationCompute::getInfoCellBuffer() const { return infoCellBuffer; }
const SimulationCompute::BufferPtr &SimulationCompute::getCellBuffer() const { return cellBuffers[currentBuffer]; }

void SimulationCompute::swapBuffers() {
  std::swap(cellBuffers[0], cellBuffers[1]);
  currentBuffer = (currentBuffer + 1) % 2;
}

std::unique_ptr<SimulationCompute> SimulationCompute::CreateInstance(SimulationType type, glm::uvec3 tankSize) {
  switch (type) {
  case SimulationType::Basic:
    return std::make_unique<BasicSimulationCompute>(tankSize);
  case SimulationType::Advanced:
    return std::make_unique<AdvancedSimulationCompute>(tankSize);
  }
}

float SimulationCompute::getFluidVolume() {

  auto ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_WRITE_ONLY));

  float volume = 0;
  for (auto i : MakeRange::range(glm::compMul(tankSize))) {
    volume += ptrWR[i].getFluidVolume();
  }
  cellBuffers[0]->unmap();
  return volume;
}