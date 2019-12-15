//
// Created by Igor Frank on 13.12.19.
//

#include "GridRenderer.h"
#include <GlslShaderLoader.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <shader_literals.h>
#include <types/Range.h>
#include <utility>

using namespace std::string_literals;
using namespace ge::gl;

std::vector<glm::vec3> GridRenderer::generateGrid(int xLength, int yLength, int zLength) {
  std::vector<glm::vec3> result{};
  using namespace MakeRange;
  for (auto [z, y] : range<int, 2>({zLength + 1, yLength + 1})) {
    result.emplace_back(glm::vec3{0.0, y, z});
    result.emplace_back(glm::vec3{xLength, y, z});
  }
  for (auto [x, y] : range<int, 2>({xLength + 1, yLength + 1})) {
    result.emplace_back(glm::vec3{x, y, 0.0});
    result.emplace_back(glm::vec3{x, y, zLength});
  }
  for (auto [x, z] : range<int, 2>({xLength + 1, zLength + 1})) {
    result.emplace_back(glm::vec3{x, 0.0, z});
    result.emplace_back(glm::vec3{x, yLength, z});
  }
  return result;
}

GridRenderer::GridRenderer(glm::vec3 gridSize, glm::mat4 projectionMat)
    : projectionMat(projectionMat), gridSize(glm::compMul(gridSize + glm::vec3(1))) {

  using namespace ShaderLiterals;
  setShaderLocation(SRC_DIR + "/Shaders"s);
  program = std::make_shared<Program>("grid"_vert, "grid"_frag);
  vbo = createBuffer(generateGrid(gridSize));
  vao->addAttrib(vbo, 0, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(glm::vec3)));
}

std::vector<glm::vec3> GridRenderer::generateGrid(glm::ivec3 length) { return generateGrid(length.x, length.y, length.z); }

void GridRenderer::draw(const glm::mat4 &viewMat) {
  program->use();
  vao->bind();
  program->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1, GL_FALSE);
  program->setMatrix4fv("View", glm::value_ptr(viewMat), 1, GL_FALSE);
  program->setMatrix4fv("Projection", glm::value_ptr(projectionMat), 1, GL_FALSE);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridSize * 2));
  vao->unbind();
}
