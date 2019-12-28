//
// Created by Igor Frank on 13.12.19.
//

#include "CellRenderer.h"
#include <GlslShaderLoader.h>
#include <IboBuffer.h>
#include <Model.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/detail/type_mat.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <shader_literals.h>
#include <utility>

using namespace std::string_literals;
using namespace ge::gl;

CellRenderer::CellRenderer([[maybe_unused]] const std::string &cellModelPath, const glm::mat4 &projectionMat,
                           const glm::uvec3 &tankSize)
    : projectionMat(projectionMat), tankSize(tankSize) {
  using namespace ShaderLiterals;
  setShaderLocation(SRC_DIR + "/Shaders"s);
  program = std::make_shared<Program>("basic"_vert, "basic"_frag, "basic"_geom);
  std::vector<glm::vec4> positions{glm::compMul(tankSize), glm::vec4(-1)};

  vbo = createBuffer(std::vector<Model::VertexData>{
      Model::VertexData{glm::vec3(0.0), glm::vec3{0.0, 0.0, 1.0}, glm::vec2(0.0), glm::vec3(0.0)}});

  vao->addAttrib(vbo, 0, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(Model::VertexData)), offsetof(Model::VertexData, pos));
  vao->addAttrib(vbo, 1, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(Model::VertexData)), offsetof(Model::VertexData, color));
}

void CellRenderer::draw(const glm::mat4 &viewMat, const glm::vec3 &cameraPos, const BufferPtr &cellBuffer,
                        const BufferPtr &cellInfoBuffer, float cellSize) {
  program->use();

  vao->bind();
  cellBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cellInfoBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  program->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1, GL_FALSE);
  program->setMatrix4fv("View", glm::value_ptr(viewMat), 1, GL_FALSE);
  program->setMatrix4fv("Projection", glm::value_ptr(projectionMat), 1, GL_FALSE);
  program->set("cellSize", cellSize);
  program->set3uiv("tankSize", glm::value_ptr(tankSize), 1);
  program->set3fv("cameraPosition", glm::value_ptr(cameraPos));
  glDrawArraysInstanced(GL_POINTS, 0, 1, glm::compMul(tankSize));
  cellBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cellInfoBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 2);
  vao->unbind();
}
