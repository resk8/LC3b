#ifdef __linux__ 
    #include <stdio.h>
    #include "../include/LC3b.h"
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

    /*TODO: I hate this implementation of joining the cs bits
    In the future, look into dependency injection pattern*/
    agex_cs_bits AGEX_CS;
    mem_cs_bits MEM_CS;
    sr_cs_bits SR_CS;
};