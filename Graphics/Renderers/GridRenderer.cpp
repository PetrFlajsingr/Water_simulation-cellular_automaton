//
// Created by Igor Frank on 13.12.19.
//

#include <types/Range.h>
#include <GlslShaderLoader.h>
#include <shader_literals.h>
#include <geGL_utils.h>
#include <geGL/StaticCalls.h>
#include "GridRenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <utility>

using namespace std::string_literals;
using namespace ge::gl;

std::vector<glm::vec3> GridRenderer::generateGrid(int xLength, int yLength, int zLength) {
    std::vector<glm::vec3> result{};
    using namespace MakeRange;
    for (auto[z, y] : range<int, 2>({zLength + 1, yLength + 1})) {
        result.emplace_back(glm::vec3{0.0, y, z});
        result.emplace_back(glm::vec3{xLength, y, z});
        std::cout << "y: " << y << " z: " << z << std::endl;
    }
    for (auto[x, y] : range<int, 2>({xLength + 1, yLength + 1})) {
        result.emplace_back(glm::vec3{x, y, 0.0});
        result.emplace_back(glm::vec3{x, y, zLength});
    }
    for (auto[x, z] : range<int, 2>({xLength + 1, zLength + 1})) {
        result.emplace_back(glm::vec3{x, 0.0, z});
        result.emplace_back(glm::vec3{x, yLength, z});
    }
    return result;
}

GridRenderer::GridRenderer(glm::vec3 gridSize, std::shared_ptr<Camera> camera, glm::mat4 projectionMat) :
        gridSize(glm::compMul(gridSize + glm::vec3(1))),
        camera(std::move(camera)),
        projectionMat(projectionMat) {

    using namespace ShaderLiterals;
    setShaderLocation(SRC_DIR + "/Shaders"s);
    program = std::make_shared<ge::gl::Program>("grid"_vert, "grid"_frag);
    vbo = createBuffer(generateGrid(gridSize));
    vao->addAttrib(vbo, 0, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(glm::vec3)));
}

std::vector<glm::vec3> GridRenderer::generateGrid(glm::ivec3 length) {
    return generateGrid(length.x, length.y, length.z);
}

void GridRenderer::draw() {
    program->use();
    vao->bind();
    program->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1,
                          GL_FALSE);
    program->setMatrix4fv("View", glm::value_ptr(camera->GetViewMatrix()), 1, GL_FALSE);
    program->setMatrix4fv("Projection", glm::value_ptr(projectionMat), 1, GL_FALSE);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridSize*2));
    vao->unbind();
}
