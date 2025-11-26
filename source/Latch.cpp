/***************************************************************/
/* Latch Implementaion                                      */
/***************************************************************/

#ifdef __linux__
    #include "../include/PipeLine.h"
    #include "../include/Latch.h"
    
#else
    #include "PipeLine.h"
    #include "Latch.h"
#endif

/*
* TODO
*/
Latch::Latch() : 
NPC(0),
DATA(0),
SR1(0),
SR2(0),
ALU_RESULT(0),
ADDRESS(0),
IR(0),
DRID(0),
CC(0),
V(false),
AGEX_CS(0),
MEM_CS(0),
SR_CS(0)
{

}

/*
* TODO
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
    AGEX_CS = latch.AGEX_CS;
    MEM_CS = latch.MEM_CS;
    SR_CS = latch.SR_CS;
}
