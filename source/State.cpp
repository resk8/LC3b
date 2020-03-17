/***************************************************************/
/* Cpu State Implementaion                                     */
/***************************************************************/

#include <iostream>
#include <cstring>
#ifdef __linux__
    #include "../include/Simulator.h"
    #include "../include/State.h"
#else
    #include "Simulator.h"
    #include "State.h"
#endif

/***************************************************************/
/*                                                             */
/* Procedure : init_state                                      */
/*                                                             */
/* Purpose   : Zero out all signals and registers              */
/*                                                             */
/***************************************************************/
void State::init_state() 
{  
  PC = 0;
  N = P = 0;
  Z = 1;
  REGS = std::vector<bits16>(LC3b_REGS);

  std::memset(&DE, 0, sizeof(PipeState_DE_stage_Struct));
  std::memset(&MEM, 0, sizeof(PipeState_MEM_stage_Struct));
  std::memset(&SR, 0,sizeof(PipeState_SR_stage_Struct));
  std::memset(&STALL, 0, sizeof(PipeState_Hazards_Struct));
}

/*
* Return the current N Z P bits and loads a new nzp value from SR stage into N Z P
*/
bits3 State::GetNZP(bool load_new_nzp)
{
  auto nzp = bits3(0);
  nzp[2] = GetNBit();
  nzp[1] = GetZBit();
  nzp[0] = GetPBit();

  if(load_new_nzp)
  {
    N = SR.sr_n;
    Z = SR.sr_z;
    P = SR.sr_p;
  }
  
  return nzp;
}

/*
* Move the data into the requested register
*/
void State::SetDataForRegister(const bits3 & reg, const bits16 & data)
{
  try
  {
    REGS.at(reg.to_num()) = data;
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Invalid Register: reg=%d\n",reg);
    printf("C++ error code : %s\n",oor.what());
    Exit();  
  }
}

/*
* return the value of the requested register
*/
bits16 State::GetRegisterData(const bits3 & reg) const
{
  try
  {
    return REGS.at(reg.to_num());
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Invalid Register: reg=%d\n",reg);
    printf("C++ error code : %s\n",oor.what());
    Exit();  
  }
}

/*
* Loads new data into destination register and return data from reg sr1 and sr2
*/
void State::ProcessRegisterFile(const bits16 & de_instruction)
{
  auto sr1 = de_instruction.range<8,6>();
  auto sr2 = bits3(0);
  if(de_instruction[13] /*SR2.IDMUX*/) 
    sr2 = de_instruction.range<11,9>();
  else
    sr2 = de_instruction.range<2,0>();

  DE.de_sr1 = GetRegisterData(sr1);
  DE.de_sr2 = GetRegisterData(sr2);

  if(SR.v_sr_ld_reg)
  {
    SetDataForRegister(SR.sr_reg_id, SR.sr_reg_data);
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current architectural state  to the       */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void State::rdump(FILE * dumpsim_file) 
{  
  printf("\nCurrent architectural state :\n");
  printf("-------------------------------------\n");
  printf("Cycle Count : %d\n", simulator().GetCycles());
  printf("CpuState.GetProgramCounter()          : 0x%04x\n", GetProgramCounter().to_num());
  printf("CCs: N = %d  Z = %d  P = %d\n", GetNBit(), GetZBit(), GetPBit());
  printf("Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
	  printf("%d: 0x%04x\n", k, GetRegisterData(k).to_num());
  }
  
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent architectural state :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Cycle Count : %d\n", simulator().GetCycles());
  fprintf(dumpsim_file, "CpuState.GetProgramCounter()          : 0x%04x\n", GetProgramCounter().to_num());
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", GetNBit(), GetZBit(), GetPBit());
  fprintf(dumpsim_file, "Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
	  fprintf(dumpsim_file, "%d: 0x%04x\n", k, GetRegisterData(k).to_num());
  }

  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}