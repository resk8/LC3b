#ifdef __linux__ 
    #include <stdio.h>
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

#define COPY_AGEX_CS_START 3 
#define COPY_MEM_CS_START  9
#define COPY_SR_CS_START   7

class PipeLine;
class Latch
{
    public:
    Latch(PipeLine & instance);
    ~Latch(){}

    PipeLine & pipeline() { return _pipe; }
    void operator=(const Latch & latch);

    private:
    PipeLine & _pipe;

    bits16  NPC,
            DATA,
            SR1,
            SR2,
            ALU_RESULT,
            ADDRESS,
            IR;
    bits3   DRID,
            CC;
    bool    V;
    cs_bits CS;
};