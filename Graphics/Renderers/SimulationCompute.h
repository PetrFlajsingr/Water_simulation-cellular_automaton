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

public:
  SimulationCompute(glm::uvec3 tankSize);

  void simulate();
  void setFluidVolume(int index, float volume);
  void setFluidVolume(glm::vec3 index, float volume);
  void setFluidVolume(std::vector<glm::uvec3> indices, std::vector<float> volumes);
  void swapBuffers();
  void reset();
  BufferPtr getCellBuffer();

private:
  void initBuffers(int size);
  ProgramPtr horizontalProgram;
  ProgramPtr verticalProgram;

  glm::uvec3 tankSize;
  std::array<BufferPtr, 2> cellBuffers;
  unsigned int currentBuffer = 0;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
