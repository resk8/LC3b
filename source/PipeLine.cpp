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
    #include "../include/PipeLine.h"
#else
    #include "Simulator.h"
    #include "State.h"
    #include "MicroSequencer.h"
    #include "MainMemory.h"
    #include "PipeLine.h"
#endif

/***************************************************************/
/*                                                             */
/* Procedure : init_pipeline                                   */
/*                                                             */
/* Purpose   : Zero out all Latches                            */
/*                                                             */
/***************************************************************/
void PipeLine::init_pipeline()
{
  std::memset(&PS, 0 ,sizeof(PipeState_Entry)); 
  std::memset(&NEW_PS, 0 , sizeof(PipeState_Entry));

  PS.AGEX_CS = NEW_PS.AGEX_CS = agex_cs_bits();
  PS.MEM_CS = NEW_PS.MEM_CS = mem_cs_bits();
  PS.SR_CS = NEW_PS.SR_CS = sr_cs_bits();
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
  printf("CpuState.GetProgramCounter()              : 0x%04x\n", cpu_state.GetProgramCounter());
  printf("CCs: N = %d  Z = %d  P = %d\n", cpu_state.GetNBit(), cpu_state.GetZBit(), cpu_state.GetPBit());
  printf("Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
	  printf("%d: 0x%04x\n", k, (cpu_state.GetRegisterData(k) & 0xFFFF));
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

  printf("------------- DE   Latches --------------\n");
  printf("DE_NPC          :  0x%04x\n", PS.DE_NPC );
  printf("DE_IR           :  0x%04x\n", PS.DE_IR );
  printf("DE_V            :  %d\n", PS.DE_V);
  printf("\n");
  
  printf("------------- AGEX Latches --------------\n");
  printf("AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
  printf("AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
  printf("AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
  printf("AGEX_CC         :  %d\n", PS.AGEX_CC );
  printf("AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
  printf("AGEX_DRID       :  %d\n", PS.AGEX_DRID);
  printf("AGEX_CS         :  ");
  for (auto k = 0 ; k < NUM_AGEX_CS_BITS; k++) 
  {
    printf("%d",PS.AGEX_CS[k]);
  }

  printf("\n");
  printf("AGEX_V          :  %d\n", PS.AGEX_V);  
  printf("\n");

  printf("------------- MEM  Latches --------------\n");
  printf("MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
  printf("MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
  printf("MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS ); 
  printf("MEM_CC          :  %d\n", PS.MEM_CC );
  printf("MEM_IR          :  0x%04x\n", PS.MEM_IR );
  printf("MEM_DRID        :  %d\n", PS.MEM_DRID);
  printf("MEM_CS          :  ");
  for (auto k = 0 ; k < NUM_MEM_CS_BITS; k++) 
  {
    printf("%d",PS.MEM_CS[k]);
  }

  printf("\n");  
  printf("MEM_V           :  %d\n", PS.MEM_V);
  printf("\n");

  printf("------------- SR   Latches --------------\n");
  printf("SR_NPC          :  0x%04x\n", PS.SR_NPC );
  printf("SR_DATA         :  0x%04x\n", PS.SR_DATA );
  printf("SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
  printf("SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
  printf("SR_IR           :  0x%04x\n", PS.SR_IR );
  printf("SR_DRID         :  %d\n", PS.SR_DRID);
  printf("SR_CS           :  ");
  for (auto k = 0 ; k < NUM_SR_CS_BITS; k++) 
  {
    printf("%d",PS.SR_CS[k]);
  }
  
  printf("\n");
  printf("SR_V            :  %d\n", PS.SR_V);    
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file,"\nCurrent architectural state :\n");
  fprintf(dumpsim_file,"-------------------------------------\n");
  fprintf(dumpsim_file,"Cycle Count     : %d\n", simulator().GetCycles());
  fprintf(dumpsim_file,"CpuState.GetProgramCounter()              : 0x%04x\n", cpu_state.GetProgramCounter());
  fprintf(dumpsim_file,"CCs: N = %d  Z = %d  P = %d\n", cpu_state.GetNBit(), cpu_state.GetZBit(), cpu_state.GetPBit());
  fprintf(dumpsim_file,"Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
    fprintf(dumpsim_file,"%d: 0x%04x\n", k, (cpu_state.GetRegisterData(k) & 0xFFFF));
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
  fprintf(dumpsim_file,"DE_NPC          :  0x%04x\n", PS.DE_NPC );
  fprintf(dumpsim_file,"DE_IR           :  0x%04x\n", PS.DE_IR );
  fprintf(dumpsim_file,"DE_V            :  %d\n", PS.DE_V);
  fprintf(dumpsim_file,"\n");
  
  fprintf(dumpsim_file,"------------- AGEX Latches --------------\n");
  fprintf(dumpsim_file,"AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
  fprintf(dumpsim_file,"AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
  fprintf(dumpsim_file,"AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
  fprintf(dumpsim_file,"AGEX_CC         :  %d\n", PS.AGEX_CC );
  fprintf(dumpsim_file,"AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
  fprintf(dumpsim_file,"AGEX_DRID       :  %d\n", PS.AGEX_DRID);
  fprintf(dumpsim_file,"AGEX_CS         :  ");
  for (auto k = 0 ; k < NUM_AGEX_CS_BITS; k++) 
  {
    fprintf(dumpsim_file,"%d",PS.AGEX_CS[k]);
  }

  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"AGEX_V          :  %d\n", PS.AGEX_V);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- MEM  Latches --------------\n");
  fprintf(dumpsim_file,"MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
  fprintf(dumpsim_file,"MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
  fprintf(dumpsim_file,"MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS ); 
  fprintf(dumpsim_file,"MEM_CC          :  %d\n", PS.MEM_CC );
  fprintf(dumpsim_file,"MEM_IR          :  0x%04x\n", PS.MEM_IR );
  fprintf(dumpsim_file,"MEM_DRID        :  %d\n", PS.MEM_DRID);
  fprintf(dumpsim_file,"MEM_CS          :  ");
  for (auto k = 0 ; k < NUM_MEM_CS_BITS; k++) 
  {
    fprintf(dumpsim_file,"%d",PS.MEM_CS[k]);
  }

  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"MEM_V           :  %d\n", PS.MEM_V);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- SR   Latches --------------\n");
  fprintf(dumpsim_file,"SR_NPC          :  0x%04x\n", PS.SR_NPC );
  fprintf(dumpsim_file,"SR_DATA         :  0x%04x\n",PS.SR_DATA );
  fprintf(dumpsim_file,"SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
  fprintf(dumpsim_file,"SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
  fprintf(dumpsim_file,"SR_IR           :  0x%04x\n", PS.SR_IR );
  fprintf(dumpsim_file,"SR_DRID         :  %d\n", PS.SR_DRID);
  fprintf(dumpsim_file,"SR_CS           :  ");
  for (auto k = 0 ; k < NUM_SR_CS_BITS; k++) 
  {
    fprintf(dumpsim_file, "%d",PS.SR_CS[k]);
  }
  
  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"SR_V            :  %d\n", PS.SR_V);
  fprintf(dumpsim_file,"\n");

  fflush(dumpsim_file);        
}

/*
* return the state of the branch logic unit
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
  NEW_PS = PS; 
  SR_stage();
  MEM_stage(); 
  AGEX_stage();
  DE_stage();
  FETCH_stage();
  PS = NEW_PS; 
}

/************************* SR_stage() *************************/
void PipeLine::SR_stage() 
{  
  MicroSequencer & micro_sequencer =  simulator().microsequencer();
  SR_Stage_Entry & sr_stage = simulator().state().SrStage();
  /* You are given the code for SR_stage to get you started. Look at
     the figure for SR stage to see how this code is implemented. */
  
  switch (micro_sequencer.Get_DR_VALUEMUX1(PS.SR_CS).to_num())
  {
  case 0: 
    sr_stage.sr_reg_data = PS.SR_ADDRESS ;
    break;
  case 1:
    sr_stage.sr_reg_data = PS.SR_DATA ;
    break;
  case 2:
    sr_stage.sr_reg_data = PS.SR_NPC ;
    break;
  case 3:
    sr_stage.sr_reg_data = PS.SR_ALU_RESULT ;
    break;
  }

  sr_stage.sr_reg_id = PS.SR_DRID; 
  sr_stage.v_sr_ld_reg = micro_sequencer.Get_SR_LD_REG(PS.SR_CS) & PS.SR_V;
  sr_stage.v_sr_ld_cc = micro_sequencer.Get_SR_LD_CC(PS.SR_CS) & PS.SR_V ;

  /* CC LOGIC  */
  sr_stage.sr_n = ((sr_stage.sr_reg_data[15] & 0x1) ? 1 : 0);
  sr_stage.sr_z = ((sr_stage.sr_reg_data.to_num() & 0xFFFF) ? 0 : 1);
  sr_stage.sr_p = ((!sr_stage.sr_n) && (!sr_stage.sr_z));
}


/************************* MEM_stage() *************************/
void PipeLine::MEM_stage() 
{
  uint16_t ii,jj = 0;
  
  /* your code for MEM stage goes here */




  
  /* The code below propagates the control signals from MEM.CS latch
     to SR.CS latch. You still need to latch other values into the
     other SR latches. */
  for (ii=COPY_SR_CS_START; ii < NUM_MEM_CS_BITS; ii++) 
  {
    NEW_PS.SR_CS [jj++] = PS.MEM_CS [ii];
  }
}


/************************* AGEX_stage() *************************/
void PipeLine::AGEX_stage() 
{
  uint16_t ii, jj = 0;
  uint16_t LD_MEM; /* You need to write code to compute the value of LD.MEM
		 signal */

  /* your code for AGEX stage goes here */

  

  if (LD_MEM) 
  {
    /* Your code for latching into MEM latches goes here */
    


    /* The code below propagates the control signals from AGEX.CS latch
       to MEM.CS latch. */
    for (ii = COPY_MEM_CS_START; ii < NUM_AGEX_CS_BITS; ii++) 
    {
      NEW_PS.MEM_CS [jj++] = PS.AGEX_CS [ii]; 
    }
  }
}



/************************* DE_stage() *************************/
void PipeLine::DE_stage() 
{
  MicroSequencer & micro_sequencer = simulator().microsequencer();
  uint16_t CONTROL_STORE_ADDRESS;  /* You need to implement the logic to
			                            set the value of this variable. Look
			                            at the figure for DE stage */ 
  auto jj = 0;
  bool LD_AGEX; /* You need to write code to compute the value of
		              LD.AGEX signal */

  /* your code for DE stage goes here */
  //auto de_ir_15_11 = (PS.DE_IR >> 11) & 0x1f;
  //auto de_ir_5 = (PS.DE_IR >> 5) & 0x1;
  //CONTROL_STORE_ADDRESS = ((de_ir_15_11 << 1) | de_ir_5) & 0x3f;

  



  if (LD_AGEX)
  {
    /*AGEX NPC*/
    NEW_PS.AGEX_NPC = PS.DE_NPC;

    /*AGEX Instruction*/
    NEW_PS.AGEX_IR = PS.DE_IR;

    /*AGEX SR1 needed*/
    NEW_PS.AGEX_SR1 = 0; //TODO

    /*AGEX SR2 needed*/
    NEW_PS.AGEX_SR2 = 0; //TODO

    /*AGEX CS*/
    NEW_PS.MEM_CC = 0;   //TODO

    /*AGEX CS bits*/
    /*The code below propagates the control signals from the CONTROL
      STORE to the AGEX.CS latch. */
    for (auto ii = COPY_AGEX_CS_START; ii< NUM_CONTROL_STORE_BITS; ii++) 
    {
      NEW_PS.AGEX_CS[jj++] = micro_sequencer.GetMicroCodeBitsAt(CONTROL_STORE_ADDRESS,ii);
    }

    /*AGEX DRID*/
    auto DR_MUX = micro_sequencer.Get_DRMUX(micro_sequencer.GetMicroCodeAt(CONTROL_STORE_ADDRESS));
    if(DR_MUX)
      NEW_PS.AGEX_DRID = 0x7;
    else
      NEW_PS.AGEX_DRID = PS.DE_IR.range<11,9>();

    /*AGEX Valid*/
    NEW_PS.AGEX_V = 0; //TODO        
  }
}

/************************* FETCH_stage() *************************/
void PipeLine::FETCH_stage() 
{
  /* your code for FETCH stage goes here */
  State & cpu_state = simulator().state();
  MEM_Stage_Entry & mem_stage = simulator().state().MemStage();
  Stall_Entry & stall = simulator().state().Stall();
  MainMemory & memory = simulator().memory();
  uint16_t new_pc, instruction;

  //get the instruction from the instruction cache and the ready bit
  memory.icache_access(cpu_state.GetProgramCounter(),instruction,stall.icache_r);

  //the de npc latch will be the address of the next instruction
  auto de_npc = cpu_state.GetProgramCounter() + 2;

  //Decide on what the next program counter will be
  switch(mem_stage.mem_pc_mux)
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
  auto ld_pc = !IsStallDetected();
  if(ld_pc)
  {    
    cpu_state.SetProgramCounter(new_pc);
  }

  //do not latch the DE in case there is a data stall or dependency stall
  auto ld_de = (stall.dep_stall || stall.mem_stall) ? 0 : 1;
  if(ld_de)
  {
    NEW_PS.DE_IR = instruction;
    NEW_PS.DE_NPC = de_npc;

    //DE.valid is 0 if stall was detected or a branch
    //was not taken. Ohterwise, stage is good to go
    NEW_PS.DE_V = (!ld_de || stall.v_mem_br_stall) ? 0 : 1;
  }
}