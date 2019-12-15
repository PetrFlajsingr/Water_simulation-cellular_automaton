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

using namespace ge::gl;

class GridRenderer {

private:
  std::shared_ptr<Program> program;

  std::shared_ptr<Buffer> vbo;

  std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();

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
