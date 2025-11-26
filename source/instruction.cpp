/***************************************************************/
/* instruction.cpp: LC-3b Instruction Class Source File        */
/***************************************************************/

#include <memory>
#ifdef __linux__ 
    #include "../include/instruction.h"
    #include "../include/Disassembler.h"
#else
    #include "instruction.h"
    #include "Disassembler.h"
#endif

/**
 * @brief Factory method to create an Instruction object managed by a unique_ptr.
 * 
 * @param instance A reference to the simulator instance.
 * @return A shared_ptr to the newly created Instruction.
 */
std::shared_ptr<Instruction> Instruction::Create(Simulator & instance, const bits16 & instruction_bits)
{
    //  std::make_shared cannot access the private constructor.
    //  We must call `new` directly inside this factory method and wrap the result.
    return std::shared_ptr<Instruction>(new Instruction(instance, instruction_bits));
}

/**
 * @brief Private constructor for the Instruction class.
 * 
 * @param instance A reference to the simulator instance.
 */
Instruction::Instruction(Simulator & instance, const bits16 & instruction_bits) : _simulator(instance), IR(instruction_bits)
{
    //  Disassemble the instruction as soon as it's created.
    disassembly = Disassembler::disassemble(instruction_bits);    
    //  Initialize other data fields to a known state.
    IR = instruction_bits;
    NPC = 0;
    DATA = 0;
    SR1 = 0;
    SR2 = 0;
    ALU_RESULT = 0;
    ADDRESS = 0;
    DRID = 0;
    CC = 0;
}
