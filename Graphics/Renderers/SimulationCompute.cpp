//
// Created by Igor Frank on 13.12.19.
//

#include "SimulationCompute.h"
#include <Cell.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/gtx/component_wise.hpp>
#include <shader_literals.h>

SimulationCompute::SimulationCompute(
    const glm::uvec3 tankSize, const std::shared_ptr<Buffer> &ibo,
    const std::shared_ptr<Buffer> &positionBuffer)
    : ibo(ibo), positionsBuffer(positionBuffer), tankSize(tankSize) {
  using namespace ShaderLiterals;
  horizontalProgram =
      std::make_shared<ge::gl::Program>("basic-horizontal"_comp);
  verticalProgram = std::make_shared<ge::gl::Program>("basic-vertical"_comp);

  initBuffers(glm::compMul(tankSize));
}

void SimulationCompute::simulate() {
  Cell *ptrRD;
  Cell *ptrWR;
  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();

  horizontalProgram->use();
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  positionsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  ibo->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

  glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();

  verticalProgram->use();
  swapBuffers();
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SimulationCompute::initBuffers(int size) {
  auto cells = std::vector<Cell>(size);
  cellBuffers = {createBuffer(cells, GL_DYNAMIC_COPY),
                 createBuffer(cells, GL_DYNAMIC_COPY)};
}

void SimulationCompute::setFluidVolume(int index, float volume) {
  Cell *ptrRD;
  Cell *ptrWR;
  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

  ptrRD[index].setFluidVolume(volume);
  ptrWR[index].setFluidVolume(volume);

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();
}

void SimulationCompute::swapBuffers() {
  std::swap(cellBuffers[0], cellBuffers[1]);
}
