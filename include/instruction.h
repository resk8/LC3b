/***************************************************************/
/* instruction.h: LC-3b Instruction Class Header File          */
/***************************************************************/
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
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
  bits16 GetPC() const { return PC; }
  int GetFetchCycle() const { return fetch_cycle; }

  // Data fields that travel with the instruction through the pipeline
  bits16 PC;         // Program Counter (instruction address)
  bits16 IR;
  bits16 NPC;
  bits16 DATA;
  bits16 SR1;
  bits16 SR2;
  bits16 ALU_RESULT;
  bits16 ADDRESS;
  bits3 DRID;
  bits3 CC;

  // Control signals
  agex_cs_bits AGEX_CS;
  mem_cs_bits MEM_CS;
  sr_cs_bits SR_CS;

  // Pipeline stage tracking for timing diagram
  int fetch_cycle;                        // Cycle when instruction was fetched
  std::map<int, std::string> cycle_history; // Map of cycle -> stage symbol
  uint16_t mem_addr;                      // Memory address (for load/store)
  bool mem_addr_valid;                    // Whether this instruction accesses memory
  std::string current_stage;              // Current pipeline stage ("F", "D", "E", "M", "S")
  
  // Stage tracking methods
  void recordStage(int cycle, const std::string& stage);
  void recordStall(int cycle, const std::string& stage);
  void setCurrentStage(const std::string& stage) { current_stage = stage; }
  std::string getCurrentStage() const { return current_stage; }

  private:
  Instruction(Simulator & instance, const bits16 & instruction_bits);
  
  Simulator & _simulator;
  std::string disassembly;
   
};