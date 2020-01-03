//
// Created by Igor Frank on 02.01.20.
//

#include "SimulationCompute.h"
#include "AdvancedSimulationCompute.h"
#include "BasicSimulationCompute.h"

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
