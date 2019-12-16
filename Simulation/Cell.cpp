//
// Created by Igor Frank on 09.12.19.
//

#include "Cell.h"

Cell::Cell(float fluidVolume, uint32_t stable, float solidVolume)
    : fluidVolume(fluidVolume), solidVolume(solidVolume), fluidHorizontalLefover(0.0), stable(stable) {}

void Cell::setFluidVolume(float fluidVolume) { Cell::fluidVolume = fluidVolume; }

void Cell::setStable(uint32_t stable) { Cell::stable = stable; }

void Cell::setSolidVolume(float solidVolume) { Cell::solidVolume = solidVolume; }
