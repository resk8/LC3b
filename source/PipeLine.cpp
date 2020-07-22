/***************************************************************/
/* PipeLine Implementaion                                      */
/***************************************************************/

#include <iostream>
#include <cstring>
#include <assert.h>
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
* //TODO
*/
PipeLine::PipeLine(Simulator & instance) : _simulator(instance) 
{
  PS = PipeState(NUM_OF_LATCHES);
  NEW_PS = PipeState(NUM_OF_LATCHES);

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
	  printf("%d: 0x%04x\n", k, cpu_state.GetRegisterData(k).to_num());
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

  auto & decode = latch(DECODE,PS);
  printf("------------- decode_sigs   Latches --------------\n");
  printf("DE_NPC          :  0x%04x\n", decode.NPC.to_num() );
  printf("DE_IR           :  0x%04x\n", decode.IR.to_num() );
  printf("DE_V            :  %d\n", decode.V);
  printf("\n");
  
  auto & agex = latch(DECODE,PS);
  printf("------------- agex_sigs Latches --------------\n");
  printf("AGEX_NPC        :  0x%04x\n", agex.NPC.to_num() );
  printf("AGEX_SR1        :  0x%04x\n", agex.SR1.to_num() );
  printf("AGEX_SR2        :  0x%04x\n", agex.SR2.to_num() );
  printf("AGEX_CC         :  %d\n", agex.CC.to_num() );
  printf("AGEX_IR         :  0x%04x\n", agex.IR.to_num() );
  printf("AGEX_DRID       :  %d\n", agex.DRID.to_num() );
  printf("AGEX_CS         :  ");
  for (auto k = 0 ; k < NUM_AGEX_CS_BITS; k++) 
  {
    printf("%d",agex.AGEX_CS[k]);
  }

  printf("\n");
  printf("AGEX_V          :  %d\n", agex.V);  
  printf("\n");

  auto & memory = latch(MEMORY,PS);
  printf("------------- memory_sigs  Latches --------------\n");
  printf("MEM_NPC         :  0x%04x\n", memory.NPC.to_num() );
  printf("MEM_ALU_RESULT  :  0x%04x\n", memory.ALU_RESULT.to_num() );
  printf("MEM_ADDRESS     :  0x%04x\n", memory.ADDRESS.to_num() ); 
  printf("MEM_CC          :  %d\n", memory.CC.to_num() );
  printf("MEM_IR          :  0x%04x\n", memory.IR.to_num() );
  printf("MEM_DRID        :  %d\n", memory.DRID.to_num() );
  printf("MEM_CS          :  ");
  for (auto k = 0 ; k < NUM_MEM_CS_BITS; k++) 
  {
    printf("%d",memory.MEM_CS[k]);
  }

  printf("\n");  
  printf("MEM_V           :  %d\n", memory.V);
  printf("\n");

  auto & store = latch(STORE,PS);
  printf("------------- store_signals   Latches --------------\n");
  printf("SR_NPC          :  0x%04x\n", store.NPC.to_num() );
  printf("SR_DATA         :  0x%04x\n", store.DATA.to_num() );
  printf("SR_ALU_RESULT   :  0x%04x\n", store.ALU_RESULT.to_num() );
  printf("SR_ADDRESS      :  0x%04x\n", store.ADDRESS.to_num() );
  printf("SR_IR           :  0x%04x\n", store.IR.to_num() );
  printf("SR_DRID         :  %d\n", store.DRID.to_num());
  printf("SR_CS           :  ");
  for (auto k = 0 ; k < NUM_SR_CS_BITS; k++) 
  {
    printf("%d",store.SR_CS[k]);
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
    fprintf(dumpsim_file,"%d: 0x%04x\n", k, cpu_state.GetRegisterData(k).to_num());
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

  fprintf(dumpsim_file,"------------- decode_sigs   Latches --------------\n");
  fprintf(dumpsim_file,"DE_NPC          :  0x%04x\n", decode.NPC.to_num() );
  fprintf(dumpsim_file,"DE_IR           :  0x%04x\n", decode.IR.to_num() );
  fprintf(dumpsim_file,"DE_V            :  %d\n", decode.V);
  fprintf(dumpsim_file,"\n");
  
  fprintf(dumpsim_file,"------------- agex_sigs Latches --------------\n");
  fprintf(dumpsim_file,"AGEX_NPC        :  0x%04x\n", agex.NPC.to_num() );
  fprintf(dumpsim_file,"AGEX_SR1        :  0x%04x\n", agex.SR1.to_num() );
  fprintf(dumpsim_file,"AGEX_SR2        :  0x%04x\n", agex.SR2.to_num() );
  fprintf(dumpsim_file,"AGEX_CC         :  %d\n", agex.CC.to_num() );
  fprintf(dumpsim_file,"AGEX_IR         :  0x%04x\n", agex.IR.to_num() );
  fprintf(dumpsim_file,"AGEX_DRID       :  %d\n", agex.DRID.to_num());
  fprintf(dumpsim_file,"AGEX_CS         :  ");
  for (auto k = 0 ; k < NUM_AGEX_CS_BITS; k++) 
  {
    fprintf(dumpsim_file,"%d",agex.AGEX_CS[k]);
  }

  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"AGEX_V          :  %d\n", agex.V);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- memory_sigs  Latches --------------\n");
  fprintf(dumpsim_file,"MEM_NPC         :  0x%04x\n", memory.NPC.to_num() );
  fprintf(dumpsim_file,"MEM_ALU_RESULT  :  0x%04x\n", memory.ALU_RESULT.to_num() );
  fprintf(dumpsim_file,"MEM_ADDRESS     :  0x%04x\n", memory.ADDRESS.to_num() ); 
  fprintf(dumpsim_file,"MEM_CC          :  %d\n", memory.CC.to_num() );
  fprintf(dumpsim_file,"MEM_IR          :  0x%04x\n", memory.IR.to_num() );
  fprintf(dumpsim_file,"MEM_DRID        :  %d\n", memory.DRID.to_num() );
  fprintf(dumpsim_file,"MEM_CS          :  ");
  for (auto k = 0 ; k < NUM_MEM_CS_BITS; k++) 
  {
    fprintf(dumpsim_file,"%d",memory.MEM_CS[k]);
  }

  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"MEM_V           :  %d\n", memory.V);
  fprintf(dumpsim_file,"\n");

  fprintf(dumpsim_file,"------------- store_signals   Latches --------------\n");
  fprintf(dumpsim_file,"SR_NPC          :  0x%04x\n", store.NPC.to_num() );
  fprintf(dumpsim_file,"SR_DATA         :  0x%04x\n",store.DATA.to_num() );
  fprintf(dumpsim_file,"SR_ALU_RESULT   :  0x%04x\n", store.ALU_RESULT.to_num() );
  fprintf(dumpsim_file,"SR_ADDRESS      :  0x%04x\n", store.ADDRESS.to_num() );
  fprintf(dumpsim_file,"SR_IR           :  0x%04x\n", store.IR.to_num() );
  fprintf(dumpsim_file,"SR_DRID         :  %d\n", store.DRID.to_num());
  fprintf(dumpsim_file,"SR_CS           :  ");
  for (auto k = 0 ; k < NUM_SR_CS_BITS; k++) 
  {
    fprintf(dumpsim_file, "%d",store.SR_CS[k]);
  }
  
  fprintf(dumpsim_file,"\n");
  fprintf(dumpsim_file,"SR_V            :  %d\n", store.V);
  fprintf(dumpsim_file,"\n");

  fflush(dumpsim_file);        
}

/*
* //TODO
*/
void PipeLine::MoveLatch(const PipeState & destination, const PipeState & source)
{
  for(auto i = 0; i < NUM_OF_LATCHES; i++)
  {
    *destination.at(i) = *source.at(i);
  }
}

/*
* //TODO
*/
Latch & PipeLine::latch(Stages stage, const PipeState & latch)
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
    assert(1); //TODO: proper error handling (should not happen)
    break;
  }
}

/*
* //TODO
*/
bool PipeLine::IsControlInstruction()
{ 
  //TODO: finish the implementation
  return 1;
}

/*
* //TODO
*/
bool PipeLine::IsOperateInstruction() 
{ 
  //TODO: finish the implementation
  return 1;
}

/*
* //TODO
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
          (stall.v_mem_br_stall && !IsBranchTaken())); //TODO: this might be wrong
}

/*
* Loads new data into destination register and return data from reg sr1 and sr2
*/
void PipeLine::ProcessRegisterFile(const bits16 & de_instruction)
{
  auto & cpu_state = simulator().state();
  auto & decode_sigs = cpu_state.DecodeStage();
  auto & store_signals = cpu_state.SrStage();

  //select the sr2 register based on the type
  //of access: register or immediate 
  //SR2.IDMUX = de_instruction[13]
  decode_sigs.de_sr1 = de_instruction.range<8,6>();
  if(de_instruction[13]) 
    decode_sigs.de_sr2 = de_instruction.range<11,9>();
  else
    decode_sigs.de_sr2 = de_instruction.range<2,0>();

  //get the data from the register
  //to be used in the Decode stage
  decode_sigs.de_sr1_data = cpu_state.GetRegisterData(decode_sigs.de_sr1);
  decode_sigs.de_sr2_data = cpu_state.GetRegisterData(decode_sigs.de_sr2);

  //load processed data into destinaion
  //register baed on ucode bit  
  if(store_signals.v_sr_ld_reg)
  {
    cpu_state.SetDataForRegister(store_signals.sr_reg_id, store_signals.sr_reg_data);
  }
}

/*
Check for any data dependency hazards
An instruction in the decode_sigs stage may require a value produced by an older instruction that
is in the agex_sigs, memory_sigs, or store_signals stage. If so, the instruction in the decode_sigs stage should be stalled, 
and a bubble should be inserted into the pipeline. Note that this implementation implies 
that we are NOT using data forwarding.
*/
bool PipeLine::CheckForDataDependencies()
{
  auto & cpu_state = simulator().state();  
  auto & ucode = simulator().microsequencer();
  auto & de_latch = latch(DECODE,PS);
  auto & agex_latch = latch(AGEX,PS);
  auto & mem_latch = latch(MEMORY,PS);
  auto & sr_latch = latch(STORE,PS);

  //check for dependecy if no stalls
  //were detected in the fetch stage
  if(de_latch.V)
  {
    auto & de_sig = cpu_state.DecodeStage();
    auto & agex_sig = cpu_state.AgexStage();    
    auto & mem_sig = cpu_state.MemStage();
    auto & sr_sig = cpu_state.SrStage();

    //Get SR1/SR2 needed bits from the control store ucode
    auto sr1_needed = ucode.Get_SR1_NEEDED(de_sig.de_ucode);
    auto sr2_needed = ucode.Get_SR2_NEEDED(de_sig.de_ucode);
    auto branch_op = ucode.Get_DE_BR_OP(de_sig.de_ucode);

    //To determine if a dependency exists, the Dependency Check Logic compares the 
    //destination register number of the instructions in the agex_sigs, memory_sigs, and store_signals stages 
    //to the source register numbers of the instruction in the decode_sigs stage. If a match 
    //is found and the instruction in the decode_sigs stage actually needs the source register
    //(as indicated by the SR1.NEEDED or SR2.NEEDED control signal) and an instruction 
    //in a later stage actually writes to the same register (as indicated by the 
    //V.agex_sigs.LD.REG, V.memory_sigs.LD.REG, or V.store_signals.LD.REG signals), DEP.STALL should be set to 1
    if(sr1_needed)
    {
      if((agex_sig.v_agex_ld_reg && (de_sig.de_sr1.to_num() == agex_latch.DRID.to_num())) ||
         (mem_sig.v_mem_ld_reg &&  (de_sig.de_sr1.to_num() == mem_latch.DRID.to_num())) || 
         (sr_sig.v_sr_ld_reg && (de_sig.de_sr1.to_num() == sr_latch.DRID.to_num())))
          return true;
    }

    if(sr2_needed)
    {
      if((agex_sig.v_agex_ld_reg && (de_sig.de_sr2.to_num() == agex_latch.DRID.to_num())) ||
         (mem_sig.v_mem_ld_reg &&  (de_sig.de_sr2.to_num() == mem_latch.DRID.to_num())) || 
         (sr_sig.v_sr_ld_reg && (de_sig.de_sr2.to_num() == sr_latch.DRID.to_num())))
          return true;
    }

    //If the instruction in the decode_sigs stage is a conditional branch instruction 
    //(as indicated by the BR.OP control signal), and if any of the instructions 
    //in the agex_sigs, memory_sigs, or store_signals stages is writing to the condition codes, then DEP.STALL
    if(branch_op && 
        (agex_sig.v_agex_ld_cc ||
         mem_sig.v_mem_ld_cc ||
         sr_sig.v_sr_ld_cc))
         return true;
  }
  return false;
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
  auto & sr_sig = simulator().state().SrStage();
  auto & store_signals = latch(STORE,PS);
  /* You are given the code for SR_stage to get you started. Look at
     the figure for store_signals stage to see how this code is implemented. */
  
  switch (micro_sequencer.Get_DR_VALUEMUX1(store_signals.SR_CS).to_num())
  {
  case 0: 
    sr_sig.sr_reg_data = store_signals.ADDRESS ;
    break;
  case 1:
    sr_sig.sr_reg_data = store_signals.DATA ;
    break;
  case 2:
    sr_sig.sr_reg_data = store_signals.NPC ;
    break;
  case 3:
    sr_sig.sr_reg_data = store_signals.ALU_RESULT ;
    break;
  }

  sr_sig.sr_reg_id = store_signals.DRID; 
  sr_sig.v_sr_ld_reg = micro_sequencer.Get_SR_LD_REG(store_signals.SR_CS) & store_signals.V;
  sr_sig.v_sr_ld_cc = micro_sequencer.Get_SR_LD_CC(store_signals.SR_CS) & store_signals.V ;

  /* CC LOGIC  */
  sr_sig.sr_n = sr_sig.sr_reg_data[15];
  sr_sig.sr_z = ((sr_sig.sr_reg_data.to_num() & 0xFFFF) ? 0 : 1);
  sr_sig.sr_p = ((!sr_sig.sr_n) && (!sr_sig.sr_z));
}

/************************* MEM_stage() *************************/
void PipeLine::MEM_stage() 
{
  SetStage(MEMORY);
  auto & store_latch = latch(STORE,NEW_PS);
  auto & memory_latch = latch(MEMORY,PS);
  
  /* your code for memory_sigs stage goes here */




  
  /* The code below propagates the control signals from memory_sigs.CS latch
     to store_signals.CS latch. You still need to latch other values into the
     other store_signals latches. */
  store_latch.SR_CS.range<3,0>() = memory_latch.MEM_CS.range<10,7>();
  
}

/************************* AGEX_stage() *************************/
void PipeLine::AGEX_stage() 
{
  SetStage(AGEX);
  auto & memory_latch = latch(STORE,NEW_PS);
  auto & agex_latch = latch(MEMORY,PS);

  uint16_t LD_MEM; /* You need to write code to compute the value of LD.memory_sigs signal */

  /* your code for agex_sigs stage goes here */

  

  if (LD_MEM) 
  {
    /* Your code for latching into memory_sigs latches goes here */
    


    /* The code below propagates the control signals from agex_sigs.CS latch
       to memory_sigs.CS latch. */
    memory_latch.MEM_CS.range<10,0>() = agex_latch.AGEX_CS.range<19,9>();
  }
}

/************************* DE_stage() *************************/
void PipeLine::DE_stage() 
{
  SetStage(DECODE);
  auto & cpu_state = simulator().state(); 
  auto & de_sig = cpu_state.DecodeStage();
  auto & stall = cpu_state.Stall();
  auto & micro_sequencer = simulator().microsequencer();
  auto & decode_latch = latch(MEMORY,PS);
  auto & agex_latch = latch(STORE,NEW_PS);
  auto CONTROL_STORE_ADDRESS = bitfield<6>(0);

  //get micro code state
  CONTROL_STORE_ADDRESS.range<5,1>() = decode_latch.IR.range<15,11>();
  CONTROL_STORE_ADDRESS[0] = decode_latch.IR[5];
  de_sig.de_ucode = micro_sequencer.GetMicroCodeAt(CONTROL_STORE_ADDRESS.to_num());

  //The instruction in the decode_sigs stage also reads the register file and the condition codes. 
  //The register file has two read ports: one for SR1 and one for SR2. decode_sigs.IR[8:6] are used 
  //to address the register file to read SR1. Either decode_sigs.IR[11:9] or decode_sigs.IR[2:0] are used to 
  //address the register file to read SR2. decode_sigs.IR[13] is used to select between 
  //decode_sigs.IR[11:9] or decode_sigs.IR[2:0]. 2At the end of the clock cycle, the SR1 value from the 
  //register file is latched into the agex_sigs.SR1 latch, and the SR2 value is latched into the 
  //agex_sigs.SR2 latch.
  ProcessRegisterFile(decode_latch.IR);
  
  //The value of the condition codes is latched into the 3-bit agex_sigs.CC latch.
  //Condition code N is stored in agex_sigs.CC[2], Z is stored in agex_sigs.CC[1], and P is stored in 
  //agex_sigs.CC[0]. Note that the condition codes and register file are read and the values 
  //obtained are latched regardless of whether an instruction needs these values.
  de_sig.de_npc = cpu_state.GetNZP(cpu_state.SrStage().v_sr_ld_cc);

  //Dependency check logic indicates whether or not the instruction in the decode_sigs stage should
  //be propagated forward. If DEP.STALL is asserted, the state of the decode_sigs latches should 
  //not be changed, and a bubble needs to be inserted into the agex_sigs stage. This is accomplished 
  //by setting the valid bit for the agex_sigs stage (agex_sigs.V) to 0. Other actions need to be taken 
  //to preserve the correct value of the PC. Therefore, the DEP.STALL signal is also used 
  //by the structures physically located in the F stage
  stall.dep_stall = CheckForDataDependencies();

  //The BR.STALL signal from the control store indicates that the instruction being processed 
  //is a control instruction, and hence the frontend of the pipeline should be stalled until 
  //this instruction updates the PC in the memory_sigs stage. In the decode_sigs stage, if decode_sigs.V is 1 and BR.STALL 
  //is 1, then the decode_sigs.BR.STALL signal should be asserted. This indicates that the instruction 
  //in the decode_sigs stage is a valid control instruction. The decode_sigs.BR.STALL signal is used to insert 
  //bubbles into the pipeline in the F stage
  stall.v_de_br_stall = decode_latch.V && micro_sequencer.Get_DE_BR_STALL(de_sig.de_ucode);

  //if mem stage is already stalled dont change the state of mem latch  
  auto LD_AGEX = (stall.mem_stall) ? 0 : 1;
  if (LD_AGEX)
  {
    /*agex_sigs NPC*/
    agex_latch.NPC = decode_latch.NPC;

    /*agex_sigs Instruction*/
    agex_latch.IR = decode_latch.IR;

    /*agex_sigs SR1 needed*/
    agex_latch.SR1 = de_sig.de_sr1_data;

    /*agex_sigs SR2 needed*/
    agex_latch.SR2 = de_sig.de_sr2_data;

    /*agex_sigs CS*/
    agex_latch.CC = de_sig.de_npc;

    /*agex_sigs CS bits*/
    agex_latch.AGEX_CS.range<19,0>() = de_sig.de_ucode.range<22,3>();

    /*agex_sigs DRID*/
    if(micro_sequencer.Get_DRMUX(de_sig.de_ucode))
      agex_latch.DRID = 0x7;
    else
      agex_latch.DRID = decode_latch.IR.range<11,9>();

    /*agex_sigs Valid: valid if no stall or bubbles were detected*/    
    agex_latch.V = (!stall.dep_stall) && (decode_latch.V);
  }
}

/************************* FETCH_stage() *************************/
void PipeLine::FETCH_stage() 
{
  SetStage(FETCH);  
  auto & cpu_state = simulator().state();
  auto & mem_stage = simulator().state().MemStage();
  auto & stall = simulator().state().Stall();
  auto & memory = simulator().memory();
  auto & decode_latch = latch(DECODE,NEW_PS);
  bits16 new_pc, instruction;

  //get the instruction from the instruction cache and the ready bit
  memory.icache_access(cpu_state.GetProgramCounter(),instruction,stall.icache_r);

  //the de npc latch will be the address of the next instruction
  auto de_npc = cpu_state.GetProgramCounter() + 2;

  //If a control instruction other than TRAP is supposed to write 
  //into the PC, the TARGET.PC value coming from the memory_sigs stage should be latched into 
  //the PC at the end of the current cycle. If a TRAP instruction is supposed to write 
  //into the PC, the TRAP.PC value coming from the memory_sigs stage should be latched into the
  //PC at the end of the current cycle. The next value to be latched into the PC is 
  //controlled by the memory_sigs.PCMUX signal, which is generated in the memory_sigs stage, and the 
  //LD.PC signal
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
  
  //if there are no stalls or control instructions in the pipeline, 
  //the PC should be incremented by 2. 
  if(!IsStallDetected())
  {    
    cpu_state.SetProgramCounter(new_pc);
  }

  //do not latch the decode_sigs in case there is a data stall or dependency stall
  auto ld_de = (stall.dep_stall || stall.mem_stall) ? 0 : 1;
  if(ld_de)
  {
    decode_latch.IR = instruction;
    decode_latch.NPC = de_npc;

    //decode_sigs.valid is 0 if stall was detected or a branch
    //was not taken. Ohterwise, stage is good to go
    decode_latch.V = (!ld_de || stall.v_mem_br_stall) ? 0 : 1;
  }
}