/***************************************************************/
#include <memory>
#include <vector>
#ifdef __linux__ 
    #include <stdio.h>
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

class Latch;
typedef std::vector<std::shared_ptr<Latch>> PipeLatches;

class Simulator;
class PipeLine
{
  public:
  PipeLine(Simulator & instance);
  ~PipeLine(){}

  Simulator & simulator() { return _simulator; }
  Latch & latch(Stages stage, const PipeLatches & latch);

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
  void MoveLatch(const PipeLatches & destination, const PipeLatches & source);
  bool IsStallDetected();
  bool IsBranchTaken();
  bool IsControlInstruction();
  bool IsOperateInstruction();
  bool IsMemoryMoveInstruction();

  private:
  Simulator & _simulator;

  /* data structure for latch */
  PipeLatches PS;
  PipeLatches NEW_PS;

  Stages current_stage;
};