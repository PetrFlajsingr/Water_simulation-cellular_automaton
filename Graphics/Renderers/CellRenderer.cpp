//
// Created by Igor Frank on 13.12.19.
//

#include <glm/detail/type_mat.hpp>
#include "CellRenderer.h"
#include <shader_literals.h>
#include <GlslShaderLoader.h>
#include <Model.h>
#include <geGL_utils.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <geGL/StaticCalls.h>

using namespace std::string_literals;

CellRenderer::CellRenderer(const std::string cellModelPath,
                           const glm::mat4 &projectionMat,
                           const glm::uvec3 &tankSize) : projectionMat(projectionMat) {
    using namespace ShaderLiterals;
    setShaderLocation(SRC_DIR + "/Shaders"s);
    program = std::make_shared<ge::gl::Program>("basic"_vert, "basic"_frag);
    std::vector<glm::vec4> positions{glm::compMul(tankSize)};
    positionsBuffer = createBuffer(positions, GL_DYNAMIC_DRAW);
    auto cell = Model(SRC_DIR + "/Resources/Models/cube.obj"s);
    vbo = createBuffer(cell.getVertices());
    ebo = createBuffer(cell.getIndices());
    std::array<uint32_t, 5> command{cell.indicesCount(), 0, 0, 0, 0};
    ibo = createBuffer(command);

    vao->addAttrib(vbo, 0, 3, GL_FLOAT,
                   static_cast<GLsizei>(sizeof(Model::VertexData)),
                   offsetof(Model::VertexData, pos));
    vao->addAttrib(vbo, 1, 3, GL_FLOAT,
                   static_cast<GLsizei>(sizeof(Model::VertexData)),
                   offsetof(Model::VertexData, color));
    vao->addAttrib(vbo, 2, 2, GL_FLOAT,
                   static_cast<GLsizei>(sizeof(Model::VertexData)),
                   offsetof(Model::VertexData, texCoord));
    vao->addAttrib(vbo, 3, 3, GL_FLOAT,
                   static_cast<GLsizei>(sizeof(Model::VertexData)),
                   offsetof(Model::VertexData, normal));

}

void CellRenderer::draw(const glm::mat4 &viewMat, const glm::vec3 &cameraPos) {
    program->use();

    vao->bind();
    ebo->bind(GL_ELEMENT_ARRAY_BUFFER);
    positionsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    program->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1,
                              GL_FALSE);
    program->setMatrix4fv("View", glm::value_ptr(viewMat), 1, GL_FALSE);
    program->setMatrix4fv("Projection", glm::value_ptr(projectionMat), 1, GL_FALSE);
    program->set3fv("cameraPosition", glm::value_ptr(cameraPos));
    ibo->bind(GL_DRAW_INDIRECT_BUFFER);
    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
    vao->unbind();
    ebo->unbind(GL_ELEMENT_ARRAY_BUFFER);
    positionsBuffer->unbind(GL_SHADER_STORAGE_BUFFER);
    ibo->unbind(GL_DRAW_INDIRECT_BUFFER);
}

const std::shared_ptr<Buffer> &CellRenderer::getIbo() const {
    return ibo;
}

const std::shared_ptr<Buffer> &CellRenderer::getPositionsBuffer() const {
    return positionsBuffer;
}
