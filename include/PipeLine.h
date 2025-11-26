/***************************************************************/
/* PipeLine.h: LC-3b Pipeline Class Header File                */
/***************************************************************/
#pragma once

#include <stdio.h>
#include <memory>
#include <vector>
#ifdef __linux__ 
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

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
  void PropagatePipeLine();
  void MoveLatch(const PipeState & destination, const PipeState & source);
  bool IsStallDetected();
  bool IsBranchTaken();
  bool IsControlInstruction();
  bool IsOperateInstruction();
  bool IsMemoryMoveInstruction();
  void ProcessRegisterFile(const bits16 & de_instruction);
  bool CheckForDataDependencies();

  private:
  Simulator & _simulator;

  /* data structure for latch */
  PipeState PS;
  PipeState NEW_PS;

  Stages current_stage;
};