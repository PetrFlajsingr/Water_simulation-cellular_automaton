//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H

#include <geGL/geGL.h>
#include <glm/glm.hpp>

class SimulationCompute {
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;
  using BufferPtr = std::shared_ptr<ge::gl::Buffer>;
  using VertexArrayPtr = std::shared_ptr<ge::gl::VertexArray>;

private:
  ProgramPtr horizontalProgram;
  ProgramPtr verticalProgram;

  BufferPtr ibo;
  BufferPtr positionsBuffer;

  glm::uvec3 tankSize;

private:
  std::array<BufferPtr, 2> cellBuffers;

public:
  SimulationCompute(glm::uvec3 tankSize, BufferPtr ibo, BufferPtr positionBuffer);

  void simulate();

  void setFluidVolume(int index, float volume);

  void swapBuffers();

private:
  void initBuffers(int size);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
