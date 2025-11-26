/***************************************************************/
/* instruction.h: LC-3b Instruction Class Header File          */
/***************************************************************/
#pragma once

#include <memory>
#ifdef __linux__ 
    #include "../include/LC3b.h"
    #include "../include/Simulator.h"
#else
    #include "LC3b.h"
    #include "Simulator.h"
#endif


class Latch;
class Instruction
{
  public:
  // Factory method to create an Instruction object
  static std::shared_ptr<Instruction> Create(Simulator & instance, const bits16 & instruction_bits);

  ~Instruction(){}

  Simulator & simulator() { return _simulator; }
  const std::string & GetDisassembly() const { return disassembly; }
  
  // Public getters for data needed by other units
  bits16 GetIR() const { return IR; }
  bits16 GetSR1() const { return SR1; }
  bits16 GetSR2() const { return SR2; }

  // Data fields that travel with the instruction through the pipeline
  bits16 IR;
  bits16 NPC;
  bits16 DATA;
  bits16 SR1;
  bits16 SR2;
  bits16 ALU_RESULT;
  bits16 ADDRESS;
  bits3 DRID;
  bits3 CC;

  private:
  Instruction(Simulator & instance, const bits16 & instruction_bits);
  
  Simulator & _simulator;
  std::string disassembly;
   
};