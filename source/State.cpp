/***************************************************************/
/* Cpu State Implementaion                                     */
/***************************************************************/

#include <iostream>
#include <cstring>
#ifdef __linux__
    #include "../include/LC3b.h"
    #include "../include/Simulator.h"
    #include "../include/State.h"
#else    
    #include "LC3b.h"
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
  PC = N = P = 0; Z = 1;
  REGS = std::vector<int>(LC3b_REGS);

  std::memset(&SR, 0 ,sizeof(PipeState_SR_stage_Struct)); 
  std::memset(&STALL, 0 , sizeof(PipeState_Hazards_Struct));
}

/*
*
*/
int State::GetRegister(int reg) const
{
  try
  {
    return REGS.at(reg);
  }
  catch (const std::out_of_range& oor)
  {
    printf("********* C++ exception *********\n");
    printf("Error: Invalid Register: reg=%d\n",reg);
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
  int k; 

  printf("\nCurrent architectural state :\n");
  printf("-------------------------------------\n");
  printf("Cycle Count : %d\n", simulator().GetCycles());
  printf("CpuState.GetProgramCounter()          : 0x%04x\n", GetProgramCounter());
  printf("CCs: N = %d  Z = %d  P = %d\n", GetNBit(), GetZBit(), GetPBit());
  printf("Registers:\n");
  for (k = 0; k < LC3b_REGS; k++)
  {
	  printf("%d: 0x%04x\n", k, (GetRegister(k) & 0xFFFF));
  }
  
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent architectural state :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Cycle Count : %d\n", simulator().GetCycles());
  fprintf(dumpsim_file, "CpuState.GetProgramCounter()          : 0x%04x\n", GetProgramCounter());
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", GetNBit(), GetZBit(), GetPBit());
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC3b_REGS; k++)
  {
	  fprintf(dumpsim_file, "%d: 0x%04x\n", k, (GetRegister(k) & 0xFFFF));
  }

  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}