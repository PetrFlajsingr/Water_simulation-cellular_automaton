//
// Created by Igor Frank on 09.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H


#include <cstdint>

class Cell {
private:
    float fluidVolume;
    float fluidHorizontalLefover;
    uint32_t stable;

public:
    void setFluidVolume(float fluidVolume);

    void setStable(uint32_t stable);

    explicit Cell(float fluidVolume = 0.0, uint32_t stable = 0);

};


#endif //GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_CELL_H
