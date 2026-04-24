/***************************************************************/
/* Cpu State Implementaion                                     */
/***************************************************************/

#include "LC3b.h"
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
  ClearExceptionPending();
  ClearPrivilegeException();
  SetTrapState(TRAP_IDLE);
  trap_ctx = TrapContext{};

  SetProgramCounter(0);
  SetPSR(0x8002); // Set the initial value of PSR with the default condition codes (Z=1)
  REGS.resize(LC3b_REGS);

  decode_sigs = {};
  agex_sigs = {};
  memory_sigs = {};
  store_sigs = {};
  stall_sigs = {};
}

/*
* Return the current N Z P bits and loads a new nzp value from SR stage into N Z P
*/
bits3 State::GetNZP()
{
  auto & store_sigs = SrSignals();
  auto nzp = GetPSR().range<2,0>();

  //load new nzp bits into cpu
  //nzp from the store stage
  if(store_sigs.v_sr_ld_cc)
  {
    PSR[2] = store_sigs.sr_n;
    PSR[1] = store_sigs.sr_z;
    PSR[0] = store_sigs.sr_p;
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
/* Purpose   : Dump current architectural state  to the        */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void State::rdump(FILE * dumpsim_file)
{
  printf("\nCurrent architectural state :\n");
  printf("-------------------------------------\n");
  printf("Cycle Count : %d\n", simulator().GetCycles());
  printf("PC          : 0x%04x\n", GetProgramCounter().to_num());
  printf("PSR         : 0x%04x (Privilege: %s)\n", GetPSR().to_num(), IsPrivilegeMode() ? "User" : "Supervisor");
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
  fprintf(dumpsim_file, "PC          : 0x%04x\n", GetProgramCounter().to_num());
  fprintf(dumpsim_file, "PSR         : 0x%04x (Privilege: %s)\n", GetPSR().to_num(), IsPrivilegeMode() ? "User" : "Supervisor");
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", GetNBit(), GetZBit(), GetPBit());
  fprintf(dumpsim_file, "Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
	  fprintf(dumpsim_file, "%d: 0x%04x\n", k, GetRegisterData(k).to_num());
  }

  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}