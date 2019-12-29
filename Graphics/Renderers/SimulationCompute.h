//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H

#include <Cell.h>
#include <CellInfo.h>
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

private:
  void initBuffers(int size);
  ProgramPtr horizontalProgram;
  ProgramPtr verticalProgram;
  ProgramPtr velocityProgram;
  ProgramPtr velocity2Program;

  glm::uvec3 tankSize;
  std::array<BufferPtr, 2> cellBuffers;
  BufferPtr infoCellBuffer;

public:
  const BufferPtr &getInfoCellBuffer() const;
private:
  unsigned int currentBuffer = 0;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
