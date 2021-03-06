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

  std::memset(&decode_sigs, 0, sizeof(PipeState_DE_stage_Struct));
  std::memset(&agex_sigs, 0, sizeof(PipeState_AGEX_stage_Struct));
  std::memset(&memory_sigs, 0, sizeof(PipeState_MEM_stage_Struct));
  std::memset(&store_sigs, 0,sizeof(PipeState_SR_stage_Struct));
  std::memset(&stall_sigs, 0, sizeof(PipeState_Hazards_Struct));
}

/*
* Return the current N Z P bits and loads a new nzp value from SR stage into N Z P
*/
bits3 State::GetNZP()
{
  auto & store_sigs = SrSignals();
  auto nzp = bits3(0);
  nzp[2] = GetNBit();
  nzp[1] = GetZBit();
  nzp[0] = GetPBit();

  //load new nzp bits into cpu
  //nzp from the store stage
  if(store_sigs.v_sr_ld_cc)
  {
    N = store_sigs.sr_n;
    Z = store_sigs.sr_z;
    P = store_sigs.sr_p;
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
    printf("Error: Invalid Register: reg=%d\n",reg.to_num());
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
    printf("Error: Invalid Register: reg=%d\n",reg.to_num());
    printf("C++ error code : %s\n",oor.what());
    Exit();
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