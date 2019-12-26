//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H

#include <Cell.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>

class SimulationCompute {
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;
  using BufferPtr = std::shared_ptr<ge::gl::Buffer>;
  using VertexArrayPtr = std::shared_ptr<ge::gl::VertexArray>;

public:
  SimulationCompute(glm::uvec3 tankSize);

  void simulate();
  void setFluidVolume(const std::vector<glm::uvec3> &indices, const std::vector<float> &volumes);
  void setFluidVolume(glm::vec3 index, float volume);
  void setFluidVolume(int index, float volume);
  void setSolid(glm::vec3 index);
  void setSolid(int index);
  void setSolid(const std::vector<glm::uvec3> &indices);
  void setSource(const std::vector<glm::uvec3> &indices);
  void setSource(glm::vec3 index);
  void setSource(int index);
  void swapBuffers();
  void reset();
  BufferPtr getCellBuffer();

private:
  void initBuffers(int size);
  void setCells(std::vector<glm::uvec3> indices, CellFlags cellType, std::vector<float> fluidVolumes,
                std::vector<float> solidVolumes);
  ProgramPtr horizontalProgram;
  ProgramPtr verticalProgram;

  glm::uvec3 tankSize;
  std::array<BufferPtr, 2> cellBuffers;
  unsigned int currentBuffer = 0;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
