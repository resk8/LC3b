/***************************************************************/
/* Operation Unit Implementaion                                */
/***************************************************************/

#include <iostream>
#ifdef __linux__    
    #include "../include/Latch.h"
    #include "../include/OperationUnit.h"
#else
    #include "Latch.h"
    #include "OperationUnit.h"
#endif

//factory methods to generate the desired logic unit
//TODO: add some way to identify which latch is being passed. Currently assume its always AGEX
std::unique_ptr<OperationUnit> OperationUnit::MakeUnit(Latch & latch)
{   
    auto alu_result_mux = latch.AGEX_CS[AGEX_ALU_RESULTMUX];
    if(alu_result_mux) 
    {
        bits16 input2;
        auto sr2_mux = latch.AGEX_CS[AGEX_SR2MUX];
        if(sr2_mux)
            input2 = latch.IR.range<4,0>().sign_ext();
        else
            input2 = latch.SR2;

        auto aluk = (latch.AGEX_CS[AGEX_ALUK1] << 1) + latch.AGEX_CS[AGEX_ALUK0];
        return std::make_unique<Alu>(latch.SR1,input2,aluk);
    }
    else
        return std::make_unique<Shifter>(latch.SR1,latch.IR.range<5,0>());
    
}

Shifter::Shifter(bits16 source, bitfield<6> control)
{

}

Alu::Alu(bits16 source1, bits16 source2, bits2 control)
{

}

