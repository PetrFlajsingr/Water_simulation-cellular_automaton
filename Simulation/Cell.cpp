//
// Created by Igor Frank on 09.12.19.
//

#include "Cell.h"

Cell::Cell(float fluidVolume, uint32_t stable) : fluidVolume(fluidVolume), stable(stable) {}

void Cell::setFluidVolume(float fluidVolume) {
    Cell::fluidVolume = fluidVolume;
}

void Cell::setStable(uint32_t stable) {
    Cell::stable = stable;
}
