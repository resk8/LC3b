/***************************************************************/
/* PipeLine Implementaion                                      */
/***************************************************************/

#include <iostream>
#include <cstring>
#ifdef __linux__
    #include "../include/Simulator.h"
    #include "../include/State.h"
    #include "../include/MicroSequencer.h"
    #include "../include/MainMemory.h"
    #include "../include/Latch.h"
    #include "../include/PipeLine.h"
#else
    #include "Simulator.h"
    #include "State.h"
    #include "MicroSequencer.h"
    #include "MainMemory.h"
    #include "Latch.h"
    #include "PipeLine.h"
#endif

/*
*
*/
PipeLine::PipeLine(Simulator & instance) : _simulator(instance) 
{
  PS = PipeLatches(NUM_OF_LATCHES);
  NEW_PS = PipeLatches(NUM_OF_LATCHES);

  for(auto i = 0; i < NUM_OF_LATCHES; i++)
  {
    PS.at(i) = std::make_shared<Latch>(*this);
    NEW_PS.at(i) = std::make_shared<Latch>(*this);
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_pipeline                                   */
/*                                                             */
/* Purpose   : Zero out all Latches                            */
/*                                                             */
/***************************************************************/
void PipeLine::init_pipeline()
{
  SetStage(UNDEFINED);
}

/***************************************************************/
/*                                                             */
/* Procedure : idump                                           */
/*                                                             */
/* Purpose   : Dump current internal state to the              */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void PipeLine::idump(FILE * dumpsim_file)
{
  State & cpu_state = simulator().state();
  
  printf("\nCurrent architectural state :\n");
  printf("-------------------------------------\n");
  printf("Cycle Count     : %d\n", simulator().GetCycles());
  printf("CpuState.GetProgramCounter()              : 0x%04x\n", cpu_state.GetProgramCounter().to_num());
  printf("CCs: N = %d  Z = %d  P = %d\n", cpu_state.GetNBit(), cpu_state.GetZBit(), cpu_state.GetPBit());
  printf("Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
	  printf("%d: 0x%04x\n", k, cpu_state.GetRegister(k).to_num());
  }
  
  printf("\n");
  
  printf("------------- Stall Signals -------------\n");
  printf("ICACHE_R        :  %d\n", cpu_state.Stall().icache_r);
  printf("DEP_STALL       :  %d\n", cpu_state.Stall().dep_stall);
  printf("V_DE_BR_STALL   :  %d\n", cpu_state.Stall().v_de_br_stall);
  printf("V_AGEX_BR_STALL :  %d\n", cpu_state.Stall().v_agex_br_stall);
  printf("MEM_STALL       :  %d\n", cpu_state.Stall().mem_stall);
  printf("V_MEM_BR_STALL  :  %d\n", cpu_state.Stall().v_mem_br_stall);    
  printf("\n");

  auto & decode = GetLatch(DECODE,PS);
  printf("------------- DE   Latches --------------\n");
  printf("DE_NPC          :  0x%04x\n", decode.NPC.to_num() );
  printf("DE_IR           :  0x%04x\n", decode.IR.to_num() );
  printf("DE_V            :  %d\n", decode.V);
  printf("\n");
  
  auto & agex = GetLatch(DECODE,PS);
  printf("------------- AGEX Latches --------------\n");
  printf("AGEX_NPC        :  0x%04x\n", agex.NPC.to_num() );
  printf("AGEX_SR1        :  0x%04x\n", agex.SR1.to_num() );
  printf("AGEX_SR2        :  0x%04x\n", agex.SR2.to_num() );
  printf("AGEX_CC         :  %d\n", agex.CC.to_num() );
  printf("AGEX_IR         :  0x%04x\n", agex.IR.to_num() );
  printf("AGEX_DRID       :  %d\n", agex.DRID.to_num() );
  printf("AGEX_CS         :  ");
  for (auto k = 0 ; k < NUM_AGEX_CS_BITS; k++) 
  {
    printf("%d",agex.CS[k]);
  }

  printf("\n");
  printf("AGEX_V          :  %d\n", agex.V);  
  printf("\n");

  auto & memory = GetLatch(MEMORY,PS);
  printf("------------- MEM  Latches --------------\n");
  printf("MEM_NPC         :  0x%04x\n", memory.NPC.to_num() );
  printf("MEM_ALU_RESULT  :  0x%04x\n", memory.ALU_RESULT.to_num() );
  printf("MEM_ADDRESS     :  0x%04x\n", memory.ADDRESS.to_num() ); 
  printf("MEM_CC          :  %d\n", memory.CC.to_num() );
  printf("MEM_IR          :  0x%04x\n", memory.IR.to_num() );
  printf("MEM_DRID        :  %d\n", memory.DRID.to_num() );
  printf("MEM_CS          :  ");
  for (auto k = 0 ; k < NUM_MEM_CS_BITS; k++) 
  {
    printf("%d",memory.CS[k]);
  }

  printf("\n");  
  printf("MEM_V           :  %d\n", memory.V);
  printf("\n");

  auto & store = GetLatch(STORE,PS);
  printf("------------- SR   Latches --------------\n");
  printf("SR_NPC          :  0x%04x\n", store.NPC.to_num() );
  printf("SR_DATA         :  0x%04x\n", store.DATA.to_num() );
  printf("SR_ALU_RESULT   :  0x%04x\n", store.ALU_RESULT.to_num() );
  printf("SR_ADDRESS      :  0x%04x\n", store.ADDRESS.to_num() );
  printf("SR_IR           :  0x%04x\n", store.IR.to_num() );
  printf("SR_DRID         :  %d\n", store.DRID.to_num());
  printf("SR_CS           :  ");
  for (auto k = 0 ; k < NUM_SR_CS_BITS; k++) 
  {
    printf("%d",store.CS[k]);
  }
  
  printf("\n");
  printf("SR_V            :  %d\n", store.V);    
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file,"\nCurrent architectural state :\n");
  fprintf(dumpsim_file,"-------------------------------------\n");
  fprintf(dumpsim_file,"Cycle Count     : %d\n", simulator().GetCycles());
  fprintf(dumpsim_file,"CpuState.GetProgramCounter()              : 0x%04x\n", cpu_state.GetProgramCounter().to_num());
  fprintf(dumpsim_file,"CCs: N = %d  Z = %d  P = %d\n", cpu_state.GetNBit(), cpu_state.GetZBit(), cpu_state.GetPBit());
  fprintf(dumpsim_file,"Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
    fprintf(dumpsim_file,"%d: 0x%04x\n", k, cpu_state.GetRegister(k).to_num());
  }
  
  fprintf(dumpsim_file,"\n");
  
  fprintf(dumpsim_file,"------------- Stall Signals -------------\n");
  fprintf(dumpsim_file,"ICACHE_R        :  %d\n", cpu_state.Stall().icache_r);
  fprintf(dumpsim_file,"DEP_STALL       :  %d\n", cpu_state.Stall().dep_stall);
  fprintf(dumpsim_file,"V_DE_BR_STALL   :  %d\n", cpu_state.Stall().v_de_br_stall);
  fprintf(dumpsim_file,"V_AGEX_BR_STALL :  %d\n", cpu_state.Stall().v_agex_br_stall);
  fprintf(dumpsim_file,"MEM_STALL       :  %d\n", cpu_state.Stall().mem_stall);
  fprintf(dumpsim_file,"V_MEM_BR_STALL  :  %d\n", cpu_state.Stall().v_mem_br_stall);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- DE   Latches --------------\n");
  fprintf(dumpsim_file,"DE_NPC          :  0x%04x\n", decode.NPC.to_num() );
  fprintf(dumpsim_file,"DE_IR           :  0x%04x\n", decode.IR.to_num() );
  fprintf(dumpsim_file,"DE_V            :  %d\n", decode.V);
  fprintf(dumpsim_file,"\n");
  
  fprintf(dumpsim_file,"------------- AGEX Latches --------------\n");
  fprintf(dumpsim_file,"AGEX_NPC        :  0x%04x\n", agex.NPC.to_num() );
  fprintf(dumpsim_file,"AGEX_SR1        :  0x%04x\n", agex.SR1.to_num() );
  fprintf(dumpsim_file,"AGEX_SR2        :  0x%04x\n", agex.SR2.to_num() );
  fprintf(dumpsim_file,"AGEX_CC         :  %d\n", agex.CC.to_num() );
  fprintf(dumpsim_file,"AGEX_IR         :  0x%04x\n", agex.IR.to_num() );
  fprintf(dumpsim_file,"AGEX_DRID       :  %d\n", agex.DRID.to_num());
  fprintf(dumpsim_file,"AGEX_CS         :  ");
  for (auto k = 0 ; k < NUM_AGEX_CS_BITS; k++) 
  {
    fprintf(dumpsim_file,"%d",agex.CS[k]);
  }

  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"AGEX_V          :  %d\n", agex.V);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- MEM  Latches --------------\n");
  fprintf(dumpsim_file,"MEM_NPC         :  0x%04x\n", memory.NPC.to_num() );
  fprintf(dumpsim_file,"MEM_ALU_RESULT  :  0x%04x\n", memory.ALU_RESULT.to_num() );
  fprintf(dumpsim_file,"MEM_ADDRESS     :  0x%04x\n", memory.ADDRESS.to_num() ); 
  fprintf(dumpsim_file,"MEM_CC          :  %d\n", memory.CC.to_num() );
  fprintf(dumpsim_file,"MEM_IR          :  0x%04x\n", memory.IR.to_num() );
  fprintf(dumpsim_file,"MEM_DRID        :  %d\n", memory.DRID.to_num() );
  fprintf(dumpsim_file,"MEM_CS          :  ");
  for (auto k = 0 ; k < NUM_MEM_CS_BITS; k++) 
  {
    fprintf(dumpsim_file,"%d",memory.CS[k]);
  }

  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"MEM_V           :  %d\n", memory.V);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- SR   Latches --------------\n");
  fprintf(dumpsim_file,"SR_NPC          :  0x%04x\n", store.NPC.to_num() );
  fprintf(dumpsim_file,"SR_DATA         :  0x%04x\n",store.DATA.to_num() );
  fprintf(dumpsim_file,"SR_ALU_RESULT   :  0x%04x\n", store.ALU_RESULT.to_num() );
  fprintf(dumpsim_file,"SR_ADDRESS      :  0x%04x\n", store.ADDRESS.to_num() );
  fprintf(dumpsim_file,"SR_IR           :  0x%04x\n", store.IR.to_num() );
  fprintf(dumpsim_file,"SR_DRID         :  %d\n", store.DRID.to_num());
  fprintf(dumpsim_file,"SR_CS           :  ");
  for (auto k = 0 ; k < NUM_SR_CS_BITS; k++) 
  {
    fprintf(dumpsim_file, "%d",store.CS[k]);
  }
  
  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"SR_V            :  %d\n", store.V);
  fprintf(dumpsim_file,"\n");

  fflush(dumpsim_file);        
}

/*
* 
*/
void PipeLine::MoveLatch(PipeLatches & latch1,PipeLatches & latch2)
{
  for(auto i = 0; i < NUM_OF_LATCHES; i++)
  {
    *latch1.at(i) = *latch2.at(i);
  }
}

/*
* 
*/
Latch & PipeLine::GetLatch(Stages stage, PipeLatches & latch)
{
  switch (stage)
  {
  case DECODE:
    return *latch.at(0);
    break;
  case AGEX:
    return *latch.at(1);
    break;
  case MEMORY:
    return *latch.at(2);
    break;
  case STORE:
    return *latch.at(3);
    break;
  default:
    static_assert(1); //TODO: proper error handling (should not happen)
    break;
  }
}

/*
* 
*/
bool PipeLine::IsControlInstruction()
{ 
  //TODO: finish the implementation
  return 1;
}

/*
* 
*/
bool PipeLine::IsOperateInstruction() 
{ 
  //TODO: finish the implementation
  return 1;
}

/*
* 
*/
bool PipeLine::IsMemoryMoveInstruction() 
{ 
  //TODO: finish the implementation
  return 1;
}

/*
* return the state of the
*/
bool PipeLine::IsBranchTaken() 
{ 
  //TODO: finish the implementation of the branch logic
  return 1;
}

/*
* Logic to detect if any stall in the pipline
*/
bool PipeLine::IsStallDetected()
{
  Stall_Entry & stall = simulator().state().Stall();
  
  //Any stall signals asserted or instruction cache is not ready.
  //For control instructions, pipeline needs to wait until the 
  //branch logic unit completes the calculation of the next PC.
  return (!stall.icache_r ||
          stall.dep_stall || 
          stall.mem_stall || 
          stall.v_agex_br_stall || 
          stall.v_de_br_stall ||          
          (stall.v_mem_br_stall && !IsBranchTaken()));
}

/*
* move the pipeline to its next stages
*/
void PipeLine::PropagatePipeLine()
{
  MoveLatch(NEW_PS,PS);
  SR_stage();
  MEM_stage(); 
  AGEX_stage();
  DE_stage();
  FETCH_stage();
  MoveLatch(PS,NEW_PS);
}

/************************* SR_stage() *************************/
void PipeLine::SR_stage() 
{  
  SetStage(STORE);

  auto & micro_sequencer =  simulator().microsequencer();
  auto & sr_stage = simulator().state().SrStage();
  auto & SR = GetLatch(STORE,PS);
  /* You are given the code for SR_stage to get you started. Look at
     the figure for SR stage to see how this code is implemented. */
  
  switch (micro_sequencer.Get_DR_VALUEMUX1(SR.CS).to_num())
  {
  case 0: 
    sr_stage.sr_reg_data = SR.ADDRESS ;
    break;
  case 1:
    sr_stage.sr_reg_data = SR.DATA ;
    break;
  case 2:
    sr_stage.sr_reg_data = SR.NPC ;
    break;
  case 3:
    sr_stage.sr_reg_data = SR.ALU_RESULT ;
    break;
  }

  sr_stage.sr_reg_id = SR.DRID; 
  sr_stage.v_sr_ld_reg = micro_sequencer.Get_SR_LD_REG(SR.CS) & SR.V;
  sr_stage.v_sr_ld_cc = micro_sequencer.Get_SR_LD_CC(SR.CS) & SR.V ;

  /* CC LOGIC  */
  sr_stage.sr_n = sr_stage.sr_reg_data[15];
  sr_stage.sr_z = ((sr_stage.sr_reg_data.to_num() & 0xFFFF) ? 0 : 1);
  sr_stage.sr_p = ((!sr_stage.sr_n) && (!sr_stage.sr_z));
}


/************************* MEM_stage() *************************/
void PipeLine::MEM_stage() 
{
  SetStage(MEMORY);
  auto & SR = GetLatch(STORE,NEW_PS);
  auto & MEM = GetLatch(MEMORY,PS);
  
  /* your code for MEM stage goes here */




  
  /* The code below propagates the control signals from MEM.CS latch
     to SR.CS latch. You still need to latch other values into the
     other SR latches. */
  SR.CS.range<3,0>() = MEM.CS.range<10,7>();
  
}


/************************* AGEX_stage() *************************/
void PipeLine::AGEX_stage() 
{
  SetStage(AGEX);
  auto & MEM = GetLatch(STORE,NEW_PS);
  auto & AGEX = GetLatch(MEMORY,PS);

  uint16_t LD_MEM; /* You need to write code to compute the value of LD.MEM
		 signal */

  /* your code for AGEX stage goes here */

  

  if (LD_MEM) 
  {
    /* Your code for latching into MEM latches goes here */
    


    /* The code below propagates the control signals from AGEX.CS latch
       to MEM.CS latch. */
    MEM.CS.range<10,0>() = AGEX.CS.range<19,9>();
  }
}



/************************* DE_stage() *************************/
void PipeLine::DE_stage() 
{
  SetStage(DECODE);
  auto & micro_sequencer = simulator().microsequencer();
  auto & AGEX = GetLatch(STORE,NEW_PS);
  auto & DE = GetLatch(MEMORY,PS);
  bitfield<6> CONTROL_STORE_ADDRESS;  /* You need to implement the logic to
			                                set the value of this variable. Look
			                                at the figure for DE stage */ 
                                      
  bool LD_AGEX; /* You need to write code to compute the value of
		              LD.AGEX signal */

  /* your code for DE stage goes here */
  CONTROL_STORE_ADDRESS.range<5,1>() = DE.IR.range<15,11>();
  CONTROL_STORE_ADDRESS[0] = DE.IR[5];

  auto u_code = micro_sequencer.GetMicroCodeAt(CONTROL_STORE_ADDRESS.to_num());



  if (LD_AGEX)
  {
    /*AGEX NPC*/
    AGEX.NPC = DE.NPC;

    /*AGEX Instruction*/
    AGEX.IR = DE.IR;

    /*AGEX SR1 needed*/
    AGEX.SR1 = 0; //TODO

    /*AGEX SR2 needed*/
    AGEX.SR2 = 0; //TODO

    /*AGEX CS*/
    AGEX.CC = 0;   //TODO

    /*AGEX CS bits*/
    AGEX.CS.range<19,0>() = u_code.range<22,3>();

    /*AGEX DRID*/
    if(micro_sequencer.Get_DRMUX(u_code))
      AGEX.DRID = 0x7;
    else
      AGEX.DRID = DE.IR.range<11,9>();

    /*AGEX Valid*/
    AGEX.V = 0; //TODO        
  }
}

/************************* FETCH_stage() *************************/
void PipeLine::FETCH_stage() 
{
  SetStage(FETCH);

  /* your code for FETCH stage goes here */
  auto & cpu_state = simulator().state();
  auto & mem_stage = simulator().state().MemStage();
  auto & stall = simulator().state().Stall();
  auto & memory = simulator().memory();
  auto & DE = GetLatch(DECODE,NEW_PS);
  bits16 new_pc, instruction;

  //get the instruction from the instruction cache and the ready bit
  memory.icache_access(cpu_state.GetProgramCounter(),instruction,stall.icache_r);

  //the de npc latch will be the address of the next instruction
  auto de_npc = cpu_state.GetProgramCounter() + 2;

  //Decide on what the next program counter will be
  switch(mem_stage.mem_pc_mux.to_num())
  {
    case 0:
      new_pc = de_npc;
      break;   
    case 1:
      new_pc = mem_stage.target_pc;
      break;
    case 2:
      new_pc = mem_stage.trap_pc;
      break;
    default:       
      break; //TODO: should not happen. add an exception?
  }
  
  //If no stall is detected then update the Program Counter
  if(!IsStallDetected())
  {    
    cpu_state.SetProgramCounter(new_pc);
  }

  //do not latch the DE in case there is a data stall or dependency stall
  auto ld_de = (stall.dep_stall || stall.mem_stall) ? 0 : 1;
  if(ld_de)
  {
    DE.IR = instruction;
    DE.NPC = de_npc;

    //DE.valid is 0 if stall was detected or a branch
    //was not taken. Ohterwise, stage is good to go
    DE.V = (!ld_de || stall.v_mem_br_stall) ? 0 : 1;
  }
}