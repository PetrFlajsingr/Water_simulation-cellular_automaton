//
// Created by Igor Frank on 09.12.19.
//

#include "Cell.h"

Cell::Cell(float fluidVolume, glm::vec4 velocity) : velocityFluidVolume(velocity.x, velocity.y, velocity.z, fluidVolume) {}

void Cell::setFluidVolume(float fluidVolume) { Cell::velocityFluidVolume.w = fluidVolume; }

std::ostream &operator<<(std::ostream &out, const Cell &cell) {
  out << " Fluid: " << cell.velocityFluidVolume.w << " Velocity: " << cell.velocityFluidVolume.x << " "
      << cell.velocityFluidVolume.y << " " << cell.velocityFluidVolume.z;
  return out;
}
float Cell::getFluidVolume() { return velocityFluidVolume.w; }
