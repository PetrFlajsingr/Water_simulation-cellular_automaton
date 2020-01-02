//
// Created by Igor Frank on 28.12.19.
//

#include "CellInfo.h"

void CellInfo::setFlags(CellFlags flag) { CellInfo::flags = flag; }

std::ostream &operator<<(std::ostream &out, const CellInfo &cell) {
  {
    switch (cell.flags) {
    case CellFlags::Solid:
      out << "[Solid]";
      break;
    case CellFlags::FluidSource:
      out << "[Source]";
      break;
    case CellFlags::NoFlag:
      out << "[Regular]";
      break;
    case CellFlags::FluidSink:
      out << "[Sink]";
      break;
    }
    out << " Leftover: " << cell.fluidVerticalLefover;
    return out;
  };
}