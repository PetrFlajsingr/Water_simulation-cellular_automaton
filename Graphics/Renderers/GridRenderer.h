//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_GRIDRENDERER_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_GRIDRENDERER_H

#include <Camera.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class GridRenderer {
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;
  using BufferPtr = std::shared_ptr<ge::gl::Buffer>;
  using VertexArrayPtr = std::shared_ptr<ge::gl::VertexArray>;
private:
  ProgramPtr program;
  BufferPtr vbo;
  VertexArrayPtr vao = std::make_shared<ge::gl::VertexArray>();

  glm::mat4 projectionMat{};
  int gridSize = 0;

public:
  GridRenderer(glm::vec3 gridSize, glm::mat4 projectionMat);

  void draw(const glm::mat4 &viewMat);

private:
  std::vector<glm::vec3> generateGrid(glm::ivec3 size);

  std::vector<glm::vec3> generateGrid(int xLength, int yLength, int zLength);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_GRIDRENDERER_H
