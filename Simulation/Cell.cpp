//
// Created by Igor Frank on 09.12.19.
//

#include "Cell.h"

Cell::Cell(float fluidVolume, glm::vec4 velocity) : fluidVolume(fluidVolume), velocity(velocity) {}

void Cell::setFluidVolume(float fluidVolume) { Cell::fluidVolume = fluidVolume; }
