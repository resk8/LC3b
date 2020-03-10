/***************************************************************/
/* Latch Implementaion                                      */
/***************************************************************/

#include <iostream>
#ifdef __linux__
    #include "../include/PipeLine.h"
    #include "../include/Latch.h"
    
#else
    #include "PipeLine.h"
    #include "Latch.h"
#endif

/*
* //TODO
*/
Latch::Latch(PipeLine & instance) : 
_pipe(instance),
NPC(bits16()),
DATA(bits16()),
SR1(bits16()),
SR2(bits16()),
ALU_RESULT(bits16()),
ADDRESS(bits16()),
IR(bits16()),
DRID(bits3()),
CC(bits3()),
V(false),
CS(cs_bits())
{

}

/*
* //TODO
*/
void Latch::operator=(const Latch & latch)
{
    NPC = latch.NPC;
    DATA = latch.DATA;
    SR1 = latch.SR1;
    SR2 = latch.SR2;
    ALU_RESULT = latch.ALU_RESULT;
    ADDRESS = latch.ADDRESS;
    IR = latch.IR;
    DRID = latch.DRID;
    CC = latch.CC;
    V = latch.V;
    CS = latch.CS;
}
