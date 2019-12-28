//
// Created by Igor Frank on 28.12.19.
//

#include "CellInfo.h"

void CellInfo::setFlags(CellFlags flag) { CellInfo::flags = flag; }

std::ostream &operator<<(std::ostream &out, const CellInfo &cell) {
  {
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
    out << " Leftover: " << cell.fluidVerticalLefover;
    return out;
  };
}