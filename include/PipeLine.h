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
  uint16_t DE_NPC,
          DE_IR,
          DE_V,
  /* AGEX lateches */
          AGEX_NPC,
          AGEX_SR1, 
          AGEX_SR2,
          AGEX_CC,
          AGEX_IR,
          AGEX_DRID,
          AGEX_V;
  std::vector<uint16_t> AGEX_CS;

  /* MEM latches */
  uint16_t MEM_NPC,
          MEM_ALU_RESULT,
          MEM_ADDRESS,
          MEM_CC,
          MEM_IR,
          MEM_DRID,
          MEM_V;
  std::vector<uint16_t> MEM_CS;

  /* SR latches */
  uint16_t SR_NPC, 
          SR_DATA,
          SR_ALU_RESULT, 
          SR_ADDRESS,
          SR_IR,
          SR_DRID,
          SR_V;
  std::vector<uint16_t> SR_CS;
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
  void FETCH_stage();
  void DE_stage();
  void AGEX_stage();
  void MEM_stage();
  void SR_stage();
  void PropagatePipeLine();
  bool IsStallDetected();
  bool IsBranchTaken();

  private:
  Simulator & _simulator;

  /* data structure for latch */
  PipeState_Entry PS, NEW_PS;
};