//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H

#include <geGL/geGL.h>
#include <glm/glm.hpp>

using namespace ge::gl;

class SimulationCompute {
private:
    std::shared_ptr<Program> horizontalProgram;

    std::shared_ptr<Program> verticalProgram;

    std::shared_ptr<Buffer> ibo;

    std::shared_ptr<Buffer> positionsBuffer;

    glm::uvec3 tankSize;

private:

    std::array<std::shared_ptr<Buffer>, 2> cellBuffers;
public:

    SimulationCompute(glm::uvec3 tankSize, const std::shared_ptr<Buffer> &ibo, const std::shared_ptr<Buffer> &positionBuffer);

    void simulate();

    void setFluidVolume(int index, float volume);

    void swapBuffers();

private:
    void initBuffers(int size);
};


#endif //GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_SIMULATIONCOMPUTE_H
