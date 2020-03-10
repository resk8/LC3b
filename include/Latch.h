#ifdef __linux__ 
    #include <stdio.h>
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

class PipeLine;
class Latch
{
    public:
    Latch(PipeLine & instance);
    ~Latch(){}

    PipeLine & pipeline() { return _pipe; }    
    void operator=(const Latch & latch);    

    //TODO: make some getters?
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

    private:
    PipeLine & _pipe;
};