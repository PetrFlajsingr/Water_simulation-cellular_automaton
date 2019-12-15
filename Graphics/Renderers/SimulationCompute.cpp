//
// Created by Igor Frank on 13.12.19.
//

#include "SimulationCompute.h"
#include <Cell.h>
#include <geGL/StaticCalls.h>
#include <geGL_utils.h>
#include <glm/gtx/component_wise.hpp>
#include <utility>
#include <shader_literals.h>
#include <IboBuffer.h>
#include <Utilities.h>
#include <types/Range.h>

using namespace ge::gl;

SimulationCompute::SimulationCompute(const glm::uvec3 tankSize, BufferPtr ibo,
                                     BufferPtr positionBuffer)
        : ibo(std::move(ibo)), positionsBuffer(std::move(positionBuffer)), tankSize(tankSize) {
    using namespace ShaderLiterals;
    horizontalProgram = std::make_shared<ge::gl::Program>("basic-horizontal"_comp);
    verticalProgram = std::make_shared<ge::gl::Program>("basic-vertical"_comp);

    initBuffers(glm::compMul(tankSize));
}

void SimulationCompute::simulate() {
    Cell *ptrRD;
    Cell *ptrWR;
/*
    ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();
*/

    horizontalProgram->use();
    cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    positionsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
    ibo->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

    glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

/*    ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();*/

    verticalProgram->use();
    swapBuffers();
    cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

/*
    ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();
*/

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SimulationCompute::initBuffers(int size) {
    auto cells = std::vector<Cell>(size);
    cellBuffers = {createBuffer(cells, GL_DYNAMIC_COPY), createBuffer(cells, GL_DYNAMIC_COPY)};
}

void SimulationCompute::setFluidVolume(int index, float volume) {
    using namespace MakeRange;

    auto index3D = Utilities::from1Dto3Dindex(index, tankSize);

    auto ptrWR = reinterpret_cast<Cell *>(cellBuffers[0]->map(GL_READ_WRITE));
    auto ptrRD = reinterpret_cast<Cell *>(cellBuffers[1]->map(GL_READ_WRITE));
    auto ptrPos = reinterpret_cast<glm::vec4 *>(positionsBuffer->map(GL_READ_WRITE));
    auto ptrIbo = reinterpret_cast<DrawElementsIndirectCommand *>(ibo->map(GL_READ_WRITE));

    ptrRD[index].setFluidVolume(volume);
    ptrWR[index].setFluidVolume(volume);
    auto count = ptrIbo->instanceCount;

    if (volume > 0.0) {
        for (auto i : range(0, count)) {
            if (ptrPos[i].x == index3D.x && ptrPos[i].y == index3D.y && ptrPos[i].z == index3D.z) {
                ptrPos[i].w = volume;
                break;
            }
            if (count == i) {
                ptrPos[count] = glm::vec4(index3D, volume);
                ptrIbo->instanceCount += 1;
            }
        }
    } else {
        std::vector<glm::vec4> newPos{glm::compMul(tankSize)};
        for (auto i : range(count)) {
            if (ptrPos[i].x == index3D.x && ptrPos[i].y == index3D.y && ptrPos[i].z == index3D.z) {
                ptrIbo->instanceCount -= 1;
                continue;
            } else {
                newPos.emplace_back(ptrPos[i]);
            }
        }
        ptrPos = newPos.data();
    }

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();
    positionsBuffer->unmap();
    ibo->unmap();
}

void SimulationCompute::swapBuffers() { std::swap(cellBuffers[0], cellBuffers[1]); }

void SimulationCompute::reset() {
    initBuffers(glm::compMul(tankSize));
    auto ptrPos = reinterpret_cast<glm::vec4 *>(positionsBuffer->map(GL_READ_WRITE));
    auto ptrIbo = reinterpret_cast<DrawElementsIndirectCommand *>(ibo->map(GL_READ_WRITE));

    ptrPos = std::vector<glm::vec4>{glm::compMul(tankSize), glm::vec4(-1)}.data();
    ptrIbo->instanceCount = 0;

    positionsBuffer->unmap();
    ibo->unmap();
}
