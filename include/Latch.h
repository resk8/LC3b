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
    #include "instruction.h"
#endif

class Instruction;

class Latch
{
    public:
    Latch();
    ~Latch(){}

    void operator=(const Latch & latch);

    // Shared pointer to the instruction object (manages all data signals)
    std::shared_ptr<Instruction> instruction;
    
    // Valid bit - indicates if this latch contains a valid instruction or a bubble
    bool V;

    // Control signals for the stage this latch feeds.
    // These are properties of the stage, not the instruction itself.
    agex_cs_bits AGEX_CS;
    mem_cs_bits MEM_CS;
    sr_cs_bits SR_CS;
};