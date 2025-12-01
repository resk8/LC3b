/***************************************************************/
/* PipeLine.h: LC-3b Pipeline Class Header File                */
/***************************************************************/
#pragma once

#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#ifdef __linux__ 
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

/*
* A structure to hold the trace of a single instruction
* as it moves through the pipeline.
*/
struct InstructionTrace {
    uint16_t pc;
    std::string disassembled;
    std::map<int, std::string> cycle_history;
    uint16_t mem_addr;
    bool mem_addr_valid;
    InstructionTrace() : pc(0), mem_addr(0), mem_addr_valid(false) {}
};

class Latch;
typedef std::vector<std::shared_ptr<Latch>> PipeState;

class Simulator;
class PipeLine
{
  public:
  PipeLine(Simulator & instance);
  ~PipeLine(){}

  Simulator & simulator() { return _simulator; }
  Latch & latch(Stages stage, const PipeState & latch);

  void idump(FILE * dumpsim_file);

  /***************************************************************/
  /* These are the functions you'll have to write.               */
  /***************************************************************/
  void init_pipeline();
  void SetStage(Stages stage) { current_stage = stage; }
  void FETCH_stage();
  void DE_stage();
  void AGEX_stage();
  void MEM_stage();
  void SR_stage();
  void Cycle();
  void PropagatePipeLine();
  void MoveLatch(const PipeState & destination, const PipeState & source);
  bool IsStallDetected();
  bool IsBranchTaken();
  bool IsControlInstruction();
  bool IsOperateInstruction();
  bool IsMemoryMoveInstruction();
  void ProcessRegisterFile(const bits16 & de_instruction);
  bool CheckForDataDependencies();
  void UpdateHistory();
  void DumpHistory();

  private:
  Simulator & _simulator;

  /* data structure for latch */
  PipeState PS;
  PipeState NEW_PS;

  Stages current_stage;

  // A vector to store the history of every instruction fetched.
  std::vector<InstructionTrace> instruction_history;
};