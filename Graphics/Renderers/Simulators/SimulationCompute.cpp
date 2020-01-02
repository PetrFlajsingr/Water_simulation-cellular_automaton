//
// Created by Igor Frank on 02.01.20.
//

#include "SimulationCompute.h"

const SimulationCompute::BufferPtr &SimulationCompute::getInfoCellBuffer() const { return infoCellBuffer; }
const SimulationCompute::BufferPtr &SimulationCompute::getCellBuffer() const { return cellBuffers[currentBuffer]; }

void SimulationCompute::swapBuffers() {
    std::swap(cellBuffers[0], cellBuffers[1]);
    currentBuffer = (currentBuffer + 1) % 2;
  }
