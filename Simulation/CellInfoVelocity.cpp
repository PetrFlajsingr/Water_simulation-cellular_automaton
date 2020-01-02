//
// Created by Igor Frank on 29.12.19.
//

#include "CellInfoVelocity.h"

void CellInfoVelocity::setFlags(const CellFlags &flags) { CellInfoVelocity::flags = flags; }

CellInfoVelocity::CellInfoVelocity(const glm::vec4 &reflectA, const glm::vec4 &reflectB, CellFlags flags)
    : reflectA(reflectA), reflectB(reflectB), flags(flags) {}
