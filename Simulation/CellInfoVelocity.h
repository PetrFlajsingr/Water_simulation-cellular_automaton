//
// Created by Igor Frank on 29.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFOVELOCITY_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFOVELOCITY_H

#include "CellInfo.h"
#include <CellFlags.h>
#include <glm/glm.hpp>

class CellInfoVelocity {
private:
  glm::vec4 reflectA;
  glm::vec4 reflectB;
  alignas(16) CellFlags flags;

public:
  explicit CellInfoVelocity(const glm::vec4 &reflectA = glm::vec4(0.0), const glm::vec4 &reflectB = glm::vec4(0.0),
                            CellFlags flags = CellFlags::NoFlag);

public:
  void setFlags(const CellFlags &flags);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELLINFOVELOCITY_H
