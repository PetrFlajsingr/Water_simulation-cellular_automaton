//
// Created by Igor Frank on 02.01.20.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H

#include <CellFlags.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <types/Range.h>

class SimulationCompute{

protected:
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;
  using BufferPtr = std::shared_ptr<ge::gl::Buffer>;
  using VertexArrayPtr = std::shared_ptr<ge::gl::VertexArray>;
  glm::uvec3 tankSize;
  std::array<BufferPtr, 2> cellBuffers;
  BufferPtr infoCellBuffer;

  virtual void simulate() = 0;
  virtual void reset() = 0;

public:
  [[nodiscard]] const BufferPtr &getInfoCellBuffer() const;
  [[nodiscard]] const BufferPtr &getCellBuffer() const;
  void swapBuffers();
  virtual void setCells(glm::vec3 index, CellFlags cellType, std::vector<float> fluidVolumes = {0.0}) = 0;
  virtual void setCells(int index, CellFlags cellType, std::vector<float> fluidVolumes = {0.0}) = 0;
  virtual void setCells(const std::vector<glm::uvec3>& indices, const CellFlags& cellType, std::vector<float> fluidVolumes = {0.0}) = 0;
  virtual void setRangeCells(MultiDimRange<unsigned int, 3> &&indices, CellFlags cellType, float fluidVolume = 0.0f) = 0;


private:
  unsigned int currentBuffer = 0;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
