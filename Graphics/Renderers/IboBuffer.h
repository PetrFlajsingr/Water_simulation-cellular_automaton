//
// Created by Igor Frank on 15.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_IBOBUFFER_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_IBOBUFFER_H

#include <geGL/Generated/OpenGLTypes.h>

struct DrawElementsIndirectCommand {
  GLuint count;
  GLuint instanceCount;
  GLuint firstIndex;
  GLuint baseVertex;
  GLuint baseInstance;
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_IBOBUFFER_H
