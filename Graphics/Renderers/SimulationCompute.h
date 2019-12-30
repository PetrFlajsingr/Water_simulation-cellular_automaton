//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H

#include <Cell.h>
#include <CellInfo.h>
#include <CellInfoVelocity.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>

class SimulationCompute {
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;
  using BufferPtr = std::shared_ptr<ge::gl::Buffer>;
  using VertexArrayPtr = std::shared_ptr<ge::gl::VertexArray>;

public:
  SimulationCompute(glm::uvec3 tankSize);

  void simulate();
  void swapBuffers();
  void reset();
  BufferPtr getCellBuffer();
  void setCells(glm::vec3 index, CellFlags cellType, std::vector<float> fluidVolumes = {0.0});
  void setCells(int index, CellFlags cellType, std::vector<float> fluidVolumes = {0.0});
  void setCells(const std::vector<glm::uvec3>& indices, const CellFlags& cellType, std::vector<float> fluidVolumes = {0.0});
  template <typename Range>
  void setRangeCells(Range &&indices, CellFlags cellType, float fluidVolume = 0.0f) {

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

private:
  void initBuffers(int size);
  ProgramPtr horizontalProgram;
  ProgramPtr verticalProgram;
  ProgramPtr velocityProgram;
  ProgramPtr velocity2Program;
  ProgramPtr velocity3Program;

  glm::uvec3 tankSize;
  std::array<BufferPtr, 2> cellBuffers;
  BufferPtr infoCellBuffer;

public:
  const BufferPtr &getInfoCellBuffer() const;
private:
  unsigned int currentBuffer = 0;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
