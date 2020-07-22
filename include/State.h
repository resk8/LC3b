#include <vector>
#ifdef __linux__ 
    #include <stdio.h>
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

/***************************************************************/
/* Definitaion of  how much LC-3b register file.               */
/***************************************************************/
#define LC3b_REGS 8

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
typedef struct PipeState_DE_stage_Struct {
  /* Signals generated by DE stage and needed by previous stages in the
    pipeline are declared below. */
  cs_bits de_ucode;
  bits16  de_sr1_data,
          de_sr2_data;
  bits3   de_sr1,
          de_sr2,
          de_npc;
} DE_Stage_Entry;

typedef struct PipeState_AGEX_stage_Struct {
  /* Signals generated by AGEX stage and needed by previous stages in the
    pipeline are declared below. */
  bool v_agex_ld_reg,
       v_agex_ld_cc;
} AGEX_Stage_Entry;

typedef struct PipeState_MEM_stage_Struct {
  /* Signals generated by MEM stage and needed by previous stages in the
    pipeline are declared below. */
  bits16 target_pc, 
         trap_pc;
  bits2  mem_pc_mux;
  bool   v_mem_ld_cc,
         v_mem_ld_reg;
} MEM_Stage_Entry;

typedef struct PipeState_SR_stage_Struct {
  /* Signals generated by SR stage and needed by previous stages in the
    pipeline are declared below. */
  bool   sr_n, 
         sr_z, 
         sr_p,
         v_sr_ld_cc,
         v_sr_ld_reg;
  bits16 sr_reg_data;
  bits3  sr_reg_id;
} SR_Stage_Entry;

typedef struct PipeState_Hazards_Struct {
  /* Internal stall signals */ 
  bool dep_stall,
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
  void SetProgramCounter(const bits16 & val) { PC = val; }
  bits16 GetProgramCounter() const {return PC; }
  bool GetNBit() const { return N; };
  bool GetPBit() const { return P; };
  bool GetZBit() const { return Z; };
  bits3 GetNZP(bool load_new_nzp);
  void SetDataForRegister(const bits3 & reg, const bits16 & data);
  bits16 GetRegisterData(const bits3 & reg) const;
  void rdump(FILE * dumpsim_file);

  Stall_Entry & Stall() { return stall_sigs; }
  AGEX_Stage_Entry & AgexStage() {return agex_sigs; }
  DE_Stage_Entry & DecodeStage() {return decode_sigs; }
  MEM_Stage_Entry & MemStage() {return memory_sigs; }
  SR_Stage_Entry & SrStage() {return store_sigs; }  
  

  private:
  Simulator & _simulator;
  /**************************************************************/
  /* The LC3-b registers                                        */
  /**************************************************************/  
  std::vector<bits16> REGS;

  /***************************************************************/
  /* architectural state                                         */
  /***************************************************************/
  bits16 PC; /* program counter */
  bool N,	   /* n condition bit */
       Z,    /* z condition bit */
       P;	   /* p condition bit */

  DE_Stage_Entry decode_sigs;
  AGEX_Stage_Entry agex_sigs;
  MEM_Stage_Entry memory_sigs;
  SR_Stage_Entry store_sigs;
  Stall_Entry stall_sigs;
};