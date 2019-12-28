//
// Created by Igor Frank on 09.12.19.
//

#include "Cell.h"

Cell::Cell(float fluidVolume, CellFlags flags, float solidVolume)
    : fluidVolume(fluidVolume), solidVolume(solidVolume), fluidVerticalLefover(0.0), flags(flags) {}

void Cell::setFluidVolume(float fluidVolume) { Cell::fluidVolume = fluidVolume; }

void Cell::setSolidVolume(float solidVolume) { Cell::solidVolume = solidVolume; }

void Cell::setFlags(CellFlags flag) { Cell::flags = flag; }

std::ostream &operator<<(std::ostream &out, const Cell &cell) { {
    switch (cell.flags) {
      case CellFlags::Cell_Solid:
        out << "[Solid]";
        break;
      case CellFlags::Cell_Source:
        out << "[Source]";
        break;
      case CellFlags::Cell_NoFLags:
        out << "[Regular]";
        break;
      case CellFlags::Cell_Sink:
        out << "[Sink]";
        break;
    }
    out << " Fluid volume: " << cell.fluidVolume <<
        " Solid volume: " << cell.solidVolume <<
        " Leftover: " << cell.fluidVerticalLefover;
    return out;
  }; }
