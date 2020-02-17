#ifdef __linux__ 
    #include <stdio.h>
#endif
#include <vector>

/***************************************************************/
/* Definitaion of  how much LC-3b register file.               */
/***************************************************************/
#define LC3b_REGS 8

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/

typedef struct PipeState_MEM_stage_Struct {
  /* Signals generated by MEM stage and needed by previous stages in the
    pipeline are declared below. */
  int target_pc, 
      trap_pc,
      mem_pc_mux;
} MEM_Stage_Entry;

typedef struct PipeState_SR_stage_Struct {
  /* Signals generated by SR stage and needed by previous stages in the
    pipeline are declared below. */
  int sr_reg_data, 
      sr_n, sr_z, sr_p,
      v_sr_ld_cc,
      v_sr_ld_reg,
      sr_reg_id;
} SR_Stage_Entry;

typedef struct PipeState_Hazards_Struct {
  /* Internal stall signals */ 
  int dep_stall,
      v_de_br_stall,
      v_agex_br_stall,
      v_mem_br_stall,
      mem_stall,
      icache_r;
} Stall_Entry;

class Simulator;
class State
{
  public:
  State(Simulator & instance) : _simulator(instance) {}
  ~State(){}

  Simulator & simulator() { return _simulator; }

  void init_state();
  void SetProgramCounter(int val);
  int GetProgramCounter() const;
  int GetNBit() const { return N; }
  int GetPBit() const { return P; }
  int GetZBit() const { return Z; }
  int GetRegisterData(int reg) const;
  void rdump(FILE * dumpsim_file);

  Stall_Entry & Stall() { return STALL; }
  SR_Stage_Entry & SrStage() {return SR; }
  MEM_Stage_Entry & MemStage() {return MEM; }  

  private:
  Simulator & _simulator;
  /**************************************************************/
  /* The LC3-b registers                                        */
  /**************************************************************/  
  std::vector<int> REGS;

  /***************************************************************/
  /* architectural state                                         */
  /***************************************************************/
  int PC,  	  /* program counter */
      N,		  /* n condition bit */
      Z,    	/* z condition bit */
      P;		  /* p condition bit */

  MEM_Stage_Entry MEM;
  SR_Stage_Entry SR;
  Stall_Entry STALL;
};