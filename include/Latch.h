/***************************************************************/
/* Latch.h: LC-3b Latch Class Header File                      */
/***************************************************************/
#pragma once

#include <memory>
#ifdef __linux__ 
    #include "../include/LC3b.h"
    #include "../include/instruction.h"
#else
    #include "LC3b.h"
#endif

class Latch
{
    public:
    Latch();
    ~Latch(){}

    void operator=(const Latch & latch);

    //TODO: make some getters?
    bits16 NPC;
    bits16 DATA;
    bits16 SR1;
    bits16 SR2;
    bits16 ALU_RESULT;
    bits16 ADDRESS;
    bits16 IR;
    bits3 DRID;
    bits3 CC;
    bool V;

    // Control signals for the stage this latch feeds.
    // These are properties of the stage, not the instruction itself.
    agex_cs_bits AGEX_CS;
    mem_cs_bits MEM_CS;
    sr_cs_bits SR_CS;
};