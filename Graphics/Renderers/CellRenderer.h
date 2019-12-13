//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLRENDERER_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLRENDERER_H


#include <vector>
#include <memory>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <Camera.h>
#include <vector>
#include <memory>
#include <geGL/geGL.h>
#include <glm/glm.hpp>
#include <Camera.h>

using namespace ge::gl;

class CellRenderer {

private:

    std::shared_ptr<Program> program;

    std::shared_ptr<Buffer> vbo;

    std::shared_ptr<Buffer> ebo;

    std::shared_ptr<Buffer> ibo;

    std::shared_ptr<Buffer> positionsBuffer;

    std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();

    glm::mat4 projectionMat{};

public:
    CellRenderer(const std::string cellModelPath, const glm::mat4 &projectionMat, const glm::uvec3 &tankSize);

    void draw(const glm::mat4 &viewMat, const glm::vec3 &cameraPos);

    const std::shared_ptr<Buffer> &getIbo() const;

    const std::shared_ptr<Buffer> &getPositionsBuffer() const;
};


#endif //GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLRENDERER_H
