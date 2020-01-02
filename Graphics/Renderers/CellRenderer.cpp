//
// Created by Igor Frank on 13.12.19.
//

#include "CellRenderer.h"
#include <GlslShaderLoader.h>
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
  programBasic = std::make_shared<Program>("basic"_vert, "basic"_frag, "basic"_geom);
  programAdvanced = std::make_shared<Program>("basic"_vert, "basic"_frag, "velocity"_geom);
  std::vector<glm::vec4> positions{glm::compMul(tankSize), glm::vec4(-1)};

  vbo = createBuffer(std::vector<Model::VertexData>{
      Model::VertexData{glm::vec3(0.0), glm::vec3{0.0, 0.0, 1.0}, glm::vec2(0.0), glm::vec3(0.0)}});

  vao->addAttrib(vbo, 0, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(Model::VertexData)), offsetof(Model::VertexData, pos));
  vao->addAttrib(vbo, 1, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(Model::VertexData)), offsetof(Model::VertexData, color));
}

float t = 0;
void CellRenderer::drawAdvanced(const glm::mat4 &viewMat, const glm::vec3 &cameraPos, const BufferPtr &cellBuffer,
                        const BufferPtr &cellInfoBuffer, bool visualizeVolumes, float cellSize) {
  programAdvanced->use();

  vao->bind();
  cellBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cellInfoBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  programAdvanced->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1, GL_FALSE);
  programAdvanced->setMatrix4fv("View", glm::value_ptr(viewMat), 1, GL_FALSE);
  programAdvanced->setMatrix4fv("Projection", glm::value_ptr(projectionMat), 1, GL_FALSE);
  programAdvanced->set("cellSize", cellSize);
  programAdvanced->set("time", t);
  programAdvanced->set3uiv("tankSize", glm::value_ptr(tankSize), 1);
  programAdvanced->set3fv("cameraPosition", glm::value_ptr(cameraPos));
  programAdvanced->set1i("showIntensity", visualizeVolumes ? 1 : 0);
  glDrawArraysInstanced(GL_POINTS, 0, 1, glm::compMul(tankSize));
  cellBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cellInfoBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 2);
  vao->unbind();

  t+=0.001;
}

void CellRenderer::drawBasic(const glm::mat4 &viewMat, const glm::vec3 &cameraPos, const BufferPtr &cellBuffer,
                        const BufferPtr &cellInfoBuffer, bool visualizeVolumes, float cellSize) {
  programBasic->use();

  vao->bind();
  cellBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cellInfoBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
  programBasic->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1, GL_FALSE);
  programBasic->setMatrix4fv("View", glm::value_ptr(viewMat), 1, GL_FALSE);
  programBasic->setMatrix4fv("Projection", glm::value_ptr(projectionMat), 1, GL_FALSE);
  programBasic->set("cellSize", cellSize);
  programBasic->set("time", t);
  programBasic->set3uiv("tankSize", glm::value_ptr(tankSize), 1);
  programBasic->set3fv("cameraPosition", glm::value_ptr(cameraPos));
  programBasic->set1i("showIntensity", visualizeVolumes ? 1 : 0);
  glDrawArraysInstanced(GL_POINTS, 0, 1, glm::compMul(tankSize));
  cellBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 1);
  cellInfoBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 2);
  vao->unbind();

  t+=0.001;
}
