//
// Created by Igor Frank on 13.12.19.
//

#include "SimulationCompute.h"
#include <Cell.h>
#include <IboBuffer.h>
#include <Utilities.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/gtx/component_wise.hpp>
#include <shader_literals.h>
#include <types/Range.h>
#include <utility>

using namespace ge::gl;

SimulationCompute::SimulationCompute(const glm::uvec3 tankSize) : tankSize(tankSize) {
  using namespace ShaderLiterals;
  horizontalProgram = std::make_shared<ge::gl::Program>("basic-horizontal"_comp);
  verticalProgram = std::make_shared<ge::gl::Program>("basic-vertical"_comp);

  initBuffers(glm::compMul(tankSize));
}

void SimulationCompute::simulate() {
  const glm::uvec3 localSizes{2, 2, 2};
  /*  [[maybe_unused]] Cell *ptrRD;
    [[maybe_unused]] Cell *ptrWR;
    ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

  verticalProgram->use();
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);

  glDispatchCompute(tankSize.x / localSizes.x, tankSize.y / localSizes.y, tankSize.z / localSizes.z);

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  /*  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

  horizontalProgram->use();
  swapBuffers();
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  glDispatchCompute(tankSize.x / localSizes.x, tankSize.y / localSizes.y, tankSize.z / localSizes.z);

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  /*  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

  swapBuffers();
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SimulationCompute::initBuffers(int size) {
  auto cells = std::vector<Cell>(size);
  cellBuffers = {createBuffer(cells, GL_DYNAMIC_COPY), createBuffer(cells, GL_DYNAMIC_COPY)};
}

void SimulationCompute::setFluidVolume(int index, float volume) {
  auto index3D = Utilities::from1Dto3Dindex(index, tankSize);
  setFluidVolume(std::vector<glm::uvec3>{index3D}, std::vector<float>{volume});
}

void SimulationCompute::setFluidVolume(glm::vec3 index, float volume) {
  setFluidVolume(std::vector<glm::uvec3>{index}, std::vector<float>{volume});
}

void SimulationCompute::setFluidVolume(std::vector<glm::uvec3> indices, std::vector<float> volumes) {
  using namespace MakeRange;

  auto ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  auto ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

  for (auto i : range(indices.size())) {
    auto linearIndex = indices[i].x + indices[i].y * tankSize.x + indices[i].z * tankSize.y * tankSize.z;
    ptrRD[linearIndex].setFluidVolume(volumes[i]);
    ptrWR[linearIndex].setFluidVolume(volumes[i]);
  }

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();
}

void SimulationCompute::swapBuffers() {
  std::swap(cellBuffers[0], cellBuffers[1]);
  currentBuffer = (currentBuffer + 1) % 2;
}

void SimulationCompute::reset() { initBuffers(glm::compMul(tankSize)); }

SimulationCompute::BufferPtr SimulationCompute::getCellBuffer() { return cellBuffers[currentBuffer]; }
