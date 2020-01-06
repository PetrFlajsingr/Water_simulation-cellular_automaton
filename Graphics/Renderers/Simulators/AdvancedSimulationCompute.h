//
// Created by Igor Frank on 13.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_ADVANCEDSIMULATIONCOMPUTE_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_ADVANCEDSIMULATIONCOMPUTE_H

#include "SimulationCompute.h"
#include <Cell.h>
#include <CellInfo.h>
#include <CellInfoVelocity.h>
#include <geGL/geGL.h>
#include <glm/glm.hpp>

class AdvancedSimulationCompute : public SimulationCompute {

public:
  explicit AdvancedSimulationCompute(glm::uvec3 tankSize);

  void simulate() override;
  void reset() override;

  void setCells(glm::vec3 index, CellFlags cellType, std::vector<float> fluidVolumes = {0.0}) override;
  void setCells(int index, CellFlags cellType, std::vector<float> fluidVolumes = {0.0}) override;
  void setCells(const std::vector<glm::uvec3> &indices, const CellFlags &cellType,
                std::vector<float> fluidVolumes = {0.0}) override;
  void setRangeCells(MultiDimRange<unsigned int, 3> &&indices, CellFlags cellType, float fluidVolume = 0.0f) override;

private:
  void initBuffers(int size);
  ProgramPtr velocityProgram;
  ProgramPtr velocity2Program;
  ProgramPtr velocity3Program;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_ADVANCEDSIMULATIONCOMPUTE_H
