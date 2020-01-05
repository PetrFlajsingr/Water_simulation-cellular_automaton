//
// Created by Igor Frank on 13.12.19.
//

#include "AdvancedSimulationCompute.h"
#include <CellInfoVelocity.h>
#include <Utilities.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <shader_literals.h>
#include <types/Range.h>

using namespace ge::gl;

AdvancedSimulationCompute::AdvancedSimulationCompute(const glm::uvec3 tankSize) {
  using namespace ShaderLiterals;
  SimulationCompute::tankSize = tankSize;
  velocityProgram = std::make_shared<ge::gl::Program>("velocity_n"_comp);
  velocity2Program = std::make_shared<ge::gl::Program>("velocity_n2"_comp);
  velocity3Program = std::make_shared<ge::gl::Program>("velocity_n3"_comp);

  initBuffers(glm::compMul(tankSize));
}

void AdvancedSimulationCompute::simulate() {
  const glm::uvec3 localSizes{4, 4, 4};
  [[maybe_unused]] Cell *ptrRD;
  [[maybe_unused]] Cell *ptrWR;
  /*
  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();*/

  velocityProgram->use();
  velocityProgram->set3v("globalSize", glm::value_ptr(tankSize));
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  infoCellBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  glDispatchCompute(std::ceil(tankSize.x / static_cast<float>(localSizes.x)),
                    std::ceil(tankSize.y / static_cast<float>(localSizes.y)),
                    std::ceil(tankSize.z / static_cast<float>(localSizes.z)));

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  velocity2Program->use();
  velocity2Program->set3v("globalSize", glm::value_ptr(tankSize));
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  infoCellBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  glDispatchCompute(std::ceil(tankSize.x / static_cast<float>(localSizes.x)),
                    std::ceil(tankSize.y / static_cast<float>(localSizes.y)),
                    std::ceil(tankSize.z / static_cast<float>(localSizes.z)));

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

  ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
  ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));
  auto ptrInfo = reinterpret_cast<CellInfo *>(infoCellBuffer->map(GL_READ_ONLY));
  auto linearIndex = 2 + 4 * tankSize.x + 2 * tankSize.y * tankSize.x;
  auto linearIndex2 = 2 + 3 * tankSize.x + 2 * tankSize.y * tankSize.x;
  auto linearIndex3 = 2 + 2 * tankSize.x + 2 * tankSize.y * tankSize.x;
  auto linearIndex4 = 2 + 1 * tankSize.x + 2 * tankSize.y * tankSize.x;
  std::cout << linearIndex << " " << ptrInfo[linearIndex] << ptrWR[linearIndex] << std::endl;
  std::cout << linearIndex2 << " " << ptrInfo[linearIndex2] << ptrWR[linearIndex2] << std::endl;
  std::cout << linearIndex3 << " " << ptrInfo[linearIndex3] << ptrWR[linearIndex3] << std::endl;
  std::cout << linearIndex4 << " " << ptrInfo[linearIndex4] << ptrWR[linearIndex4] << std::endl;
  std::cout << "--------" << std::endl;
  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();
  infoCellBuffer->unmap();

  velocity3Program->use();
  velocity3Program->set3v("globalSize", glm::value_ptr(tankSize));
  cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
  cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  infoCellBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  glDispatchCompute(std::ceil(tankSize.x / static_cast<float>(localSizes.x)),
                    std::ceil(tankSize.y / static_cast<float>(localSizes.y)),
                    std::ceil(tankSize.z / static_cast<float>(localSizes.z)));

  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);



  swapBuffers();
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void AdvancedSimulationCompute::initBuffers(int size) {
  auto cells = std::vector<Cell>(size);
  // auto infoCells = std::vector<CellInfo>(size);
  auto infoCellsVelocity = std::vector<CellInfoVelocity>(size);
  cellBuffers = {createBuffer(cells, GL_DYNAMIC_COPY), createBuffer(cells, GL_DYNAMIC_COPY)};
  infoCellBuffer = createBuffer(infoCellsVelocity, GL_DYNAMIC_COPY);
}

void AdvancedSimulationCompute::reset() { initBuffers(glm::compMul(tankSize)); }

void AdvancedSimulationCompute::setCells(int index, CellFlags cellType, std::vector<float> fluidVolumes) {
  auto index3D = Utilities::from1Dto3Dindex(index, tankSize);
  setCells(std::vector<glm::uvec3>{index3D}, cellType, fluidVolumes);
}

void AdvancedSimulationCompute::setCells(glm::vec3 index, CellFlags cellType, std::vector<float> fluidVolumes) {
  setCells(std::vector<glm::uvec3>{index}, cellType, fluidVolumes);
}

void AdvancedSimulationCompute::setCells(const std::vector<glm::uvec3> &indices, const CellFlags &cellType,
                                         std::vector<float> fluidVolumes) {
  using namespace MakeRange;

  if (cellType == CellFlags::Solid || cellType == CellFlags::FluidSink) {
    fluidVolumes = std::vector<float>(indices.size(), 0.0);
  } else if (cellType == CellFlags::FluidSource) {
    fluidVolumes = std::vector<float>(indices.size(), 1.0);
  }

  auto ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_WRITE_ONLY));
  auto ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_WRITE_ONLY));
  auto ptrInfo = reinterpret_cast<CellInfoVelocity *>(infoCellBuffer->map(GL_WRITE_ONLY));

  for (auto i : range(indices.size())) {
    auto linearIndex = indices[i].x + indices[i].y * tankSize.x + indices[i].z * tankSize.y * tankSize.x;
    ptrRD[linearIndex].setFluidVolume(fluidVolumes[i]);
    ptrInfo[linearIndex].setFlags(cellType);
    ptrWR[linearIndex].setFluidVolume(fluidVolumes[i]);
    ptrInfo[linearIndex].setFlags(cellType);
  }

  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();
  infoCellBuffer->unmap();
}
void AdvancedSimulationCompute::setRangeCells(MultiDimRange<unsigned int, 3> &&indices, CellFlags cellType, float fluidVolume) {

  if (cellType == CellFlags::Solid || cellType == CellFlags::FluidSink) {
    fluidVolume = 0.0f;
  } else if (cellType == CellFlags::FluidSource) {
    fluidVolume = 1.0f;
  }

  auto ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_WRITE_ONLY));
  auto ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_WRITE_ONLY));
  auto ptrInfo = reinterpret_cast<CellInfoVelocity *>(infoCellBuffer->map(GL_WRITE_ONLY));

  for (auto [x, y, z] : indices) {
    auto linearIndex = x + y * tankSize.x + z * tankSize.y * tankSize.x;
    ptrRD[linearIndex].setFluidVolume(fluidVolume);
    ptrInfo[linearIndex].setFlags(cellType);
    ptrWR[linearIndex].setFluidVolume(fluidVolume);
    ptrInfo[linearIndex].setFlags(cellType);
  }
  cellBuffers[0]->unmap();
  cellBuffers[1]->unmap();
  infoCellBuffer->unmap();
}
