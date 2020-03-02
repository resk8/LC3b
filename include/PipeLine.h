/***************************************************************/
#include <vector>
#ifdef __linux__ 
    #include <stdio.h>
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

#define COPY_AGEX_CS_START 3 
#define COPY_MEM_CS_START  9
#define COPY_SR_CS_START   7

typedef struct PipeState_Entry_Struct{  
  /* DE latches */
  bits16 DE_NPC,
         DE_IR;
  bool   DE_V;
  /* AGEX lateches */
  bits16 AGEX_NPC,
         AGEX_SR1, 
         AGEX_SR2,
         AGEX_IR;
  bits3  AGEX_CC,
         AGEX_DRID;        
  bool   AGEX_V;
  agex_cs_bits AGEX_CS;

  /* MEM latches */
  bits16 MEM_NPC,
         MEM_ALU_RESULT,
         MEM_ADDRESS,
         MEM_IR;
  bits3  MEM_CC,
         MEM_DRID;
  bool   MEM_V;
  mem_cs_bits MEM_CS;

  /* SR latches */
  bits16 SR_NPC, 
         SR_DATA,
         SR_ALU_RESULT, 
         SR_ADDRESS,
         SR_IR;
  bits3  SR_DRID;
  bool   SR_V;
  sr_cs_bits SR_CS;
} PipeState_Entry;

class Simulator;
class PipeLine
{
  public:
  PipeLine(Simulator & instance) : _simulator(instance) {}
  ~PipeLine(){}

  Simulator & simulator() { return _simulator; }

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
  bool IsStallDetected();
  bool IsBranchTaken();
  bool IsControlInstruction();
  bool IsOperateInstruction();
  bool IsMemoryMoveInstruction();

  private:
  Simulator & _simulator;

  /* data structure for latch */
  PipeState_Entry PS, NEW_PS;

  Stages current_stage;
};