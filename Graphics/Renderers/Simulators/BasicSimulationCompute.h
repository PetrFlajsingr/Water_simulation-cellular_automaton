//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASICSIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASICSIMULATIONCOMPUTE_H

#include "SimulationCompute.h"
#include <Cell.h>
#include <CellInfo.h>
#include <CellInfoVelocity.h>
#include <Renderers/Simulators/SimulationCompute.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>

class BasicSimulationCompute : public SimulationCompute {

public:
  explicit BasicSimulationCompute(glm::uvec3 tankSize);

  void simulate() override;
  void reset() override;
  void setCells(glm::vec3 index, CellFlags cellType, std::vector<float> fluidVolumes = {1.0}) override;
  void setCells(int index, CellFlags cellType, std::vector<float> fluidVolumes = {1.0}) override;
  void setCells(const std::vector<glm::uvec3> &indices, const CellFlags &cellType,
                std::vector<float> fluidVolumes = {1.0}) override;
  void setRangeCells(MultiDimRange<unsigned int, 3> &&indices, CellFlags cellType, float fluidVolume = 1.0) override;

private:
  void initBuffers(int size);
  ProgramPtr horizontalProgram;
  ProgramPtr verticalProgram;
  unsigned int currentBuffer = 0;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_BASICSIMULATIONCOMPUTE_H
