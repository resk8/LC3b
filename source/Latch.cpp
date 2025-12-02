/***************************************************************/
/* Latch Implementaion                                      */
/***************************************************************/

#ifdef __linux__
    #include "../include/PipeLine.h"
    #include "../include/Latch.h"
    #include "../include/instruction.h"
    
#else
    #include "PipeLine.h"
    #include "Latch.h"
    #include "instruction.h"
#endif

/*
* Initialize latch with default values
*/
Latch::Latch() : 
AGEX_CS(0),
MEM_CS(0),
SR_CS(0),
instruction(nullptr),
V(false)
{

}

/*
* Copy assignment operator
*/
void Latch::operator=(const Latch & latch)
{
    AGEX_CS = latch.AGEX_CS;
    MEM_CS = latch.MEM_CS;
    SR_CS = latch.SR_CS;
    instruction = latch.instruction;  // Shared pointer copy
    V = latch.V;
}
