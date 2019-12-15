//
// Created by Igor Frank on 05.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UTILITIES_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UTILITIES_H

#include <string>
#include <glm/glm.hpp>

class Utilities {
public:
  static std::string readFile(const std::string &filename);

  static glm::uvec3 from1Dto3Dindex(const unsigned int index, const glm::uvec3 size);
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_UTILITIES_H
