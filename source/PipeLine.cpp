/***************************************************************/
/* PipeLine Implementaion                                      */
/***************************************************************/

#include <cstring>
#include <assert.h>
#ifdef __linux__
    #include "../include/Simulator.h"
    #include "../include/State.h"
    #include "../include/MicroSequencer.h"
    #include "../include/MainMemory.h"
    #include "../include/Latch.h"
    #include "../include/OperationUnit.h"
    #include "../include/PipeLine.h"
    //#include "../include/instruction.h"
    #include "../include/Disassembler.h"
#else
    #include "Simulator.h"
    #include "State.h"
    #include "MicroSequencer.h"
    #include "MainMemory.h"
    #include "Latch.h"
    #include "OperationUnit.h"
    #include "PipeLine.h"
    //#include "instruction.h"
    #include "Disassembler.h"
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
    PS.at(i) = std::make_shared<Latch>();
    NEW_PS.at(i) = std::make_shared<Latch>();
  }
  instruction_history.reserve(100); // Pre-allocate space for performance
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
  instruction_history.clear();
}

/***************************************************************/
/*                                                             */
/* Procedure : idump                                           */
/*                                                             */
/* Purpose   : Dump pipeline timing diagram to the output file.*/
/*                                                             */
/***************************************************************/
void PipeLine::idump(FILE * dumpsim_file)
{
  // Helper macro to print to both terminal and file, removing redundancy.
  #define PRINT_AND_DUMP(...) \
      do { \
          printf(__VA_ARGS__); \
          if (dumpsim_file) { fprintf(dumpsim_file, __VA_ARGS__); } \
      } while (0)

  const int PC_COL_WIDTH = 8;
  const int INST_COL_WIDTH = 30;
  const int MEM_ADDR_COL_WIDTH = 10;
  const int CYCLE_COL_WIDTH = 5;
  int current_cycle = simulator().GetCycles();

  // Header
  PRINT_AND_DUMP("\n%-*s| %-*s| %-*s", PC_COL_WIDTH, "PC", INST_COL_WIDTH, "Instruction", MEM_ADDR_COL_WIDTH, "Mem Addr");
  for (int i = 0; i <= current_cycle; ++i) {
      char cycle_header[10];
      snprintf(cycle_header, sizeof(cycle_header), "C%d", i);
      PRINT_AND_DUMP("| %-*s", CYCLE_COL_WIDTH, cycle_header);
  }
  PRINT_AND_DUMP("|\n");

  // Separator
  std::string separator(PC_COL_WIDTH, '-');
  separator += "+" + std::string(INST_COL_WIDTH + 1, '-') + "+" + std::string(MEM_ADDR_COL_WIDTH + 1, '-');
  for (int i = 0; i <= current_cycle; ++i) {
      separator += "+" + std::string(CYCLE_COL_WIDTH + 1, '-');
  }
  separator += "|";
  PRINT_AND_DUMP("%s\n", separator.c_str());

  // Instruction Rows
  for (auto inst_iter = instruction_history.begin(); inst_iter != instruction_history.end(); ) {
      const auto& inst_trace = *inst_iter;
      char pc_str[PC_COL_WIDTH];
      snprintf(pc_str, sizeof(pc_str), "0x%04x", inst_trace.pc);

      char mem_addr_str[MEM_ADDR_COL_WIDTH + 1] = "";
      if (inst_trace.mem_addr_valid) {
          snprintf(mem_addr_str, sizeof(mem_addr_str), "0x%04x", inst_trace.mem_addr);
      }

      PRINT_AND_DUMP("%-*s| %-*s| %-*s", PC_COL_WIDTH, pc_str, INST_COL_WIDTH, inst_trace.disassembled.c_str(), MEM_ADDR_COL_WIDTH, mem_addr_str);

      bool retired = false;
      for (int i = 0; i <= current_cycle; ++i) {
          std::string stage_char = ""; // Default to an empty cell

          if (!retired) {
              auto it = inst_trace.cycle_history.find(i);
              if (it != inst_trace.cycle_history.end()) {
                  stage_char = it->second;
                  // If the instruction just completed the Store stage, mark it as retired for the *next* cycle.
                  if (it->second == "S") {
                    retired = true;
                  }
              }
          }

          PRINT_AND_DUMP("| %-*s", CYCLE_COL_WIDTH, stage_char.c_str());
      }
      PRINT_AND_DUMP("|\n");

      // If the instruction is retired, remove it from history after printing.
      if (retired) {
          inst_iter = instruction_history.erase(inst_iter);
      } else {
          ++inst_iter;
      }
  }
  fflush(dumpsim_file);

  // Undefine the macro to avoid polluting other functions
  #undef PRINT_AND_DUMP
}

/***************************************************************/
/*                                                             */
/* Procedure : Cycle                                           */
/*                                                             */
/* Purpose   : Execute one cycle of the pipeline               */
/*                                                             */
/***************************************************************/
void PipeLine::Cycle()
{
  // 1. Simulate all stages to determine the state of NEW_PS
  PropagatePipeLine();
  // 2. Record what happened in this cycle based on PS and NEW_PS
  UpdateHistory();
  // 3. Advance the pipeline by committing the new state
  MoveLatch(PS, NEW_PS);
}

void PipeLine::MoveLatch(const PipeState & destination, const PipeState & source)
{
  for(auto i = 0; i < NUM_OF_LATCHES; i++)
  {
    *destination.at(i) = *source.at(i);
  }
}

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
    throw std::runtime_error("Invalid stage for latch access"); // Should not happen
    break;
  }
}

bool PipeLine::IsControlInstruction()
{
  //TODO: finish the implementation
  return 1;
}

bool PipeLine::IsOperateInstruction()
{
  //TODO: finish the implementation
  return 1;
}

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
  auto & memory_sig = simulator().state().MemSignals();
  return (memory_sig.mem_pc_mux.to_num() != 0);
}

/*
* Logic to detect if any stall in the pipline
*/
bool PipeLine::IsStallDetected()
{
  auto & stall = simulator().state().Stall();

  // Any stall signals asserted or instruction cache is not ready.
  // For control instructions, pipeline needs to wait until the
  // branch logic unit completes the calculation of the next PC.

  if (stall.icache_r &&
      !stall.dep_stall &&
      !stall.mem_stall &&
      !stall.v_agex_br_stall &&
      !stall.v_de_br_stall &&
      !stall.v_mem_br_stall)
    return false;
  else if(stall.v_mem_br_stall && IsBranchTaken())
    return false;
  else
    return true;
}

/*
* Loads new data into destination register and return data from reg sr1 and sr2
*/
void PipeLine::ProcessRegisterFile(const bits16 & de_instruction)
{
  auto & cpu_state = simulator().state();
  auto & decode_sigs = cpu_state.DecodeSignals();
  auto & store_signals = cpu_state.SrSignals();

  // select the sr2 register based on the type
  // of access: register or immediate
  // SR2.IDMUX = de_instruction[13]
  decode_sigs.de_sr1 = de_instruction.range<8,6>();
  if(de_instruction[13])
    decode_sigs.de_sr2 = de_instruction.range<11,9>();
  else
    decode_sigs.de_sr2 = de_instruction.range<2,0>();

  // get the data from the register
  // to be used in the Decode stage
  decode_sigs.de_sr1_data = cpu_state.GetRegisterData(decode_sigs.de_sr1);
  decode_sigs.de_sr2_data = cpu_state.GetRegisterData(decode_sigs.de_sr2);

  // load processed data into destinaion
  // register baed on ucode bit
  if(store_signals.v_sr_ld_reg)
  {
    cpu_state.SetDataForRegister(store_signals.sr_drid, store_signals.sr_reg_data);
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

  // check for dependecy if no stalls
  // were detected in the fetch stage
  auto inst_de = de_latch.instruction;
  if(inst_de && de_latch.V)
  {
    auto & de_sig = cpu_state.DecodeSignals();
    auto & agex_sig = cpu_state.AgexSignals();
    auto & mem_sig = cpu_state.MemSignals();
    auto & sr_sig = cpu_state.SrSignals();

    // Get SR1/SR2 needed bits from the control store ucode
    auto sr1_needed = ucode.Get_SR1_NEEDED(de_sig.de_ucode);
    auto sr2_needed = ucode.Get_SR2_NEEDED(de_sig.de_ucode);
    auto branch_op = ucode.Get_DE_BR_OP(de_sig.de_ucode);

    // To determine if a dependency exists, the Dependency Check Logic compares the
    // destination register number of the instructions in the agex_sigs, memory_sigs, and store_signals stages
    // to the source register numbers of the instruction in the decode_sigs stage. If a match
    // is found and the instruction in the decode_sigs stage actually needs the source register
    // (as indicated by the SR1.NEEDED or SR2.NEEDED control signal) and an instruction
    // in a later stage actually writes to the same register (as indicated by the
    // V.agex_sigs.LD.REG, V.memory_sigs.LD.REG, or V.store_signals.LD.REG signals), DEP.STALL should be set to 1
    if(sr1_needed)
    {
      if((agex_sig.v_agex_ld_reg && (de_sig.de_sr1.to_num() == agex_sig.agex_drid.to_num())) ||
         (mem_sig.v_mem_ld_reg &&  (de_sig.de_sr1.to_num() == mem_sig.mem_drid.to_num())) ||
         (sr_sig.v_sr_ld_reg && (de_sig.de_sr1.to_num() == sr_sig.sr_drid.to_num())))
          return true;
    }

    if(sr2_needed)
    {
      if((agex_sig.v_agex_ld_reg && (de_sig.de_sr2.to_num() == agex_sig.agex_drid.to_num())) ||
         (mem_sig.v_mem_ld_reg &&  (de_sig.de_sr2.to_num() == mem_sig.mem_drid.to_num())) ||
         (sr_sig.v_sr_ld_reg && (de_sig.de_sr2.to_num() == sr_sig.sr_drid.to_num())))
          return true;
    }

    // If the instruction in the decode_sigs stage is a conditional branch instruction
    // (as indicated by the BR.OP control signal), and if any of the instructions
    // in the agex_sigs, memory_sigs, or store_signals stages is writing to the condition codes, then DEP.STALL
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
  // This function just simulates the logic of each stage for one cycle.
  // It calculates the state of NEW_PS based on the state of PS.
  SR_stage();
  MEM_stage();
  AGEX_stage();
  DE_stage();
  FETCH_stage();
}

void PipeLine::UpdateHistory()
{
  int current_cycle = simulator().GetCycles();
  Disassembler disassembler;

  // A new instruction has been successfully fetched if it's valid in the next
  // DECODE latch, AND it's different from what was in the current DECODE latch
  // (or the current latch was invalid). This prevents creating duplicate rows for stalls.
  auto & de_new_latch = latch(DECODE, NEW_PS);
  auto de_new_inst = de_new_latch.instruction;
  if (de_new_inst && de_new_latch.V) {
      bool is_new_instruction = false;
      // A simple check to see if this PC is already the last one in our history.
      if (instruction_history.empty() || instruction_history.back().pc != (de_new_inst->NPC.to_num() - 2)) {
          is_new_instruction = true;
      }
      auto & de_curr_latch = latch(DECODE, PS);
      auto de_curr_inst = de_curr_latch.instruction;
      if (!de_curr_inst || !de_curr_latch.V || (de_curr_inst->NPC.to_num() != de_new_inst->NPC.to_num())) {
          is_new_instruction = true;
      }

      if (is_new_instruction) {
          InstructionTrace new_trace;
          new_trace.pc = de_new_inst->NPC.to_num() - 2;
          new_trace.disassembled = disassembler.disassemble(de_new_inst->IR);
          instruction_history.push_back(new_trace);
      }
  }

  auto & micro_sequencer = simulator().microsequencer();

  // Update history for all instructions based on where they are in the CURRENT pipeline state (PS)
  for (auto& inst_trace : instruction_history) {
      std::string stage_char = " "; // Default to blank
      std::shared_ptr<Instruction> current_inst = nullptr;
      
      // Check each pipeline stage for this instruction
      auto & sr_latch = latch(STORE, PS);
      auto & mem_latch_hist = latch(MEMORY, PS);
      auto & agex_latch_hist = latch(AGEX, PS);
      auto & de_latch_hist = latch(DECODE, PS);
      
      auto sr_inst = sr_latch.instruction;
      auto mem_inst = mem_latch_hist.instruction;
      auto agex_inst = agex_latch_hist.instruction;
      auto de_inst = de_latch_hist.instruction;
      
      // Find which stage contains this instruction by matching PC
      if (sr_inst && sr_latch.V && (sr_inst->NPC.to_num() - 2 == inst_trace.pc)) {
          current_inst = sr_inst;
          stage_char = current_inst->current_stage;
          current_inst->recordStage(current_cycle, stage_char);
      }
      else if (mem_inst && mem_latch_hist.V && (mem_inst->NPC.to_num() - 2 == inst_trace.pc)) {
          current_inst = mem_inst;
          stage_char = current_inst->current_stage;
          // Check if this instruction is performing a memory access in this cycle
          if (micro_sequencer.Get_DCACHE_EN(mem_inst->MEM_CS)) {
              inst_trace.mem_addr = mem_inst->ADDRESS.to_num();
              inst_trace.mem_addr_valid = true;
              mem_inst->mem_addr = inst_trace.mem_addr;
              mem_inst->mem_addr_valid = true;
          }
          current_inst->recordStage(current_cycle, stage_char);
      }
      else if (agex_inst && agex_latch_hist.V && (agex_inst->NPC.to_num() - 2 == inst_trace.pc)) {
          current_inst = agex_inst;
          stage_char = current_inst->current_stage;
          current_inst->recordStage(current_cycle, stage_char);
      }
      else if (de_inst && de_latch_hist.V && (de_inst->NPC.to_num() - 2 == inst_trace.pc)) {
          current_inst = de_inst;
          stage_char = current_inst->current_stage;
          current_inst->recordStage(current_cycle, stage_char);
      }
      else if (inst_trace.cycle_history.empty()) {
          stage_char = "F";
      }

      // Check for stalls by seeing if the instruction is still in the same stage in the *next* cycle
      auto & de_new_latch_check = latch(DECODE, NEW_PS);
      auto de_new_inst_check = de_new_latch_check.instruction;
      auto & agex_new_latch = latch(AGEX, NEW_PS);
      auto agex_new_inst = agex_new_latch.instruction;
      auto & mem_new_latch = latch(MEMORY, NEW_PS);
      auto mem_new_inst = mem_new_latch.instruction;
      
      if (stage_char == "D" && de_new_inst_check && de_new_latch_check.V && (de_new_inst_check->NPC.to_num() - 2 == inst_trace.pc)) {
          stage_char = "D*";
          if (de_inst) {
              de_inst->recordStall(current_cycle, "D");
          }
      }
      if (stage_char == "E" && agex_new_inst && agex_new_latch.V && (agex_new_inst->NPC.to_num() - 2 == inst_trace.pc)) {
          stage_char = "E*";
          if (agex_inst) {
              agex_inst->recordStall(current_cycle, "E");
          }
      }
      if (stage_char == "M" && mem_new_inst && mem_new_latch.V && (mem_new_inst->NPC.to_num() - 2 == inst_trace.pc)) {
          stage_char = "M*";
          if (mem_inst) {
              mem_inst->recordStall(current_cycle, "M");
          }
      }

      inst_trace.cycle_history[current_cycle] = stage_char;
  }
}

/************************* SR_stage() *************************/
void PipeLine::SR_stage()
{
  SetStage(STORE);
  auto & micro_sequencer =  simulator().microsequencer();
  auto & sr_sig = simulator().state().SrSignals();
  auto & store_latch = latch(STORE,PS);
  auto inst = store_latch.instruction;
  
  /* You are given the code for SR_stage to get you started. Look at
     the figure for store_signals stage to see how this code is implemented. */

  if (inst) {
    inst->current_stage = "S";
    switch (micro_sequencer.Get_DR_VALUEMUX(inst->SR_CS).to_num())
    {
    case 0:
      sr_sig.sr_reg_data = inst->ADDRESS;
      break;
    case 1:
      sr_sig.sr_reg_data = inst->DATA;
      break;
    case 2:
      sr_sig.sr_reg_data = inst->NPC;
      break;
    case 3:
      sr_sig.sr_reg_data = inst->ALU_RESULT;
      break;
    }

    sr_sig.sr_drid = inst->DRID;
    sr_sig.v_sr_ld_reg = micro_sequencer.Get_SR_LD_REG(inst->SR_CS) & store_latch.V;
    sr_sig.v_sr_ld_cc = micro_sequencer.Get_SR_LD_CC(inst->SR_CS) & store_latch.V;

    /* CC LOGIC  */
    sr_sig.sr_n = sr_sig.sr_reg_data[15];
    sr_sig.sr_z = ((sr_sig.sr_reg_data.to_num() == 0) ? 1 : 0);
    sr_sig.sr_p = ((!sr_sig.sr_n) && (!sr_sig.sr_z));
  }
}

/************************* MEM_stage() *************************/
void PipeLine::MEM_stage()
{
  SetStage(MEMORY);
  auto & store_latch = latch(STORE,NEW_PS);
  auto & memory_latch = latch(MEMORY,PS);
  auto inst = memory_latch.instruction;
  auto & main_memory = simulator().memory();
  auto & memory_sig = simulator().state().MemSignals();
  auto & stall_sig = simulator().state().Stall();
  auto & micro_seq = simulator().microsequencer();

  if (!inst) {
    // No instruction in MEMORY - this shouldn't happen, but handle gracefully
    return;
  }
  
  inst->current_stage = "M";
  
  //access aligment logic
  auto alignment_needed = inst->ADDRESS[0];
  auto data_size = micro_seq.Get_DATA_SIZE(inst->MEM_CS);
  bits16 MDR_IN;
  if(!data_size) //byte access
  {
    bits8 alu_result = inst->ALU_RESULT.range<7,0>();
    if(alignment_needed)
      MDR_IN.range<15,8>() = alu_result.range<7,0>();
    else
    {
      //TODO: had to do this nonsense because the compiler does
      //not like bitfield<N>.range<same> = bitfield<N>.range<same>.
      //the problem might be with the range copy constructor and
      //range operator= not sure why
      MDR_IN.range<7,0>() = alu_result.range<7,0>();
    }
  }
  else //wrod access
    MDR_IN = inst->ALU_RESULT;

  //read/write enable logic
  auto read_write_en = micro_seq.Get_DCACHE_RW(memory_latch.MEM_CS);
  auto we_high = 0, we_low = 0;
  if(read_write_en)
  {
    if(data_size) //word
      we_high = we_low = 1;
    else
    {
      if(alignment_needed)
      {
        we_high = 1; we_low = 0;
      }
      else
      {
        we_high = 0, we_low = 1;
      }
    }
  }

  //data cache access
  bits16 MDR_OUT;
  bool data_cache_r = false;
  auto & memory_latch_ps = latch(MEMORY, PS);
  auto cache_en = micro_seq.Get_DCACHE_EN(inst->MEM_CS) && memory_latch_ps.V;
  if(cache_en)
    main_memory.dcache_access(inst->ADDRESS, MDR_OUT, MDR_IN, data_cache_r, we_low, we_high);
  else
    data_cache_r = true; //no stall since memory was not even accessed

  //memory signals for fetch stage
  memory_sig.target_pc = inst->ADDRESS;
  memory_sig.mem_drid = inst->DRID;
  stall_sig.mem_stall = cache_en && (!data_cache_r);

  //Branch Logic
  auto memory_v = memory_latch_ps.V;
  if(memory_v)
  {
    auto is_branch_op = micro_seq.Get_BR_OP(inst->MEM_CS);
    auto is_trap_op = micro_seq.Get_TRAP_OP(inst->MEM_CS);
    if(is_branch_op)
    {
      bits3 br_intr_nzp = inst->IR.range<11,9>();
      bits3 cpu_nzp = inst->CC;
      if(((br_intr_nzp[2] & cpu_nzp[2]) == 1) || // N
         ((br_intr_nzp[1] & cpu_nzp[1]) == 1) || // Z
         ((br_intr_nzp[0] & cpu_nzp[0]) == 1))   // P
         {
           memory_sig.mem_pc_mux = 1; //branch taken, jump to target PC
         }
    }
    else if (is_trap_op)
      memory_sig.mem_pc_mux = 2; //trap was triggered
    else
      memory_sig.mem_pc_mux = 0; //branch not taken, continue next instruction
  }
  else
    memory_sig.mem_pc_mux = 0; //memory not valid

  //process trap
  memory_sig.trap_pc = 0;
  if(cache_en)
  {
    if(data_size) //word
      memory_sig.trap_pc = MDR_OUT;
    else //byte
    {
      bits8 trap_byte;
      if(alignment_needed)
        trap_byte = MDR_OUT.range<15,8>();
      else
        trap_byte = MDR_OUT.range<7,0>();

      memory_sig.trap_pc.range<7,0>() = trap_byte.range<7,0>();
      if(memory_sig.trap_pc[7])
        memory_sig.trap_pc = memory_sig.trap_pc.sign_ext(7);
    }
  }

  //check for dependencies
  memory_sig.v_mem_ld_cc = memory_v && micro_seq.Get_MEM_LD_CC(inst->MEM_CS);
  memory_sig.v_mem_ld_reg = memory_v && micro_seq.Get_MEM_LD_REG(inst->MEM_CS);
  stall_sig.v_mem_br_stall = memory_v && micro_seq.Get_MEM_BR_STALL(inst->MEM_CS);

  //load SR latch - only control signals
  /* The code below propagates the control signals from memory_sigs.CS latch
     to store_signals.CS latch. */
  inst->SR_CS = inst->MEM_CS.range<10,7>();
  
  // Propagate instruction object and update its data fields
  bool store_valid = memory_v && (!stall_sig.mem_stall);
  store_latch.instruction = inst;
  store_latch.V = store_valid;
  inst->DATA = memory_sig.trap_pc;
}

/************************* AGEX_stage() *************************/
void PipeLine::AGEX_stage()
{
  SetStage(AGEX);
  auto & memory_latch = latch(MEMORY,NEW_PS);
  auto & agex_latch = latch(AGEX,PS);
  auto inst = agex_latch.instruction;
  auto & agex_sig = simulator().state().AgexSignals();
  auto & stall_sig = simulator().state().Stall();
  auto & micro_seq = simulator().microsequencer();

  if (!inst) {
    // No instruction in AGEX - this shouldn't happen, but handle gracefully
    return;
  }
  
  inst->current_stage = "E";
  
  /* your code for agex_sigs stage goes here */
  // First program counter mux
  bits16 next_pc_1;
  if(micro_seq.Get_ADDR1MUX(inst->AGEX_CS))
    next_pc_1 = inst->SR1;
  else
    next_pc_1 = inst->NPC;

  // second program counter mux
  // TODO: this might be broken because of the bit access to non_const overload
  bits16 next_pc_2;
  auto agex_addr2mux = micro_seq.Get_ADDR2MUX(inst->AGEX_CS);
  switch (agex_addr2mux.to_num())
  {
  case 0:
    next_pc_2 = 0;
    break;
  case 1:
    next_pc_2 = inst->IR.sign_ext(5);
    break;
  case 2:
    next_pc_2 = inst->IR.sign_ext(8);
    break;
  case 3:
    next_pc_2 = inst->IR.sign_ext(10);
    break;
  default:
    assert(true); //TODO: should not happen. add an exception?
  }

  //check if lshf1 signal is 1
  if(micro_seq.Get_LSHF1(inst->AGEX_CS))
    next_pc_2 = next_pc_2 << 1;

  //generate next instruction address
  bits16 mem_address;
  if(micro_seq.Get_ADDRESSMUX(inst->AGEX_CS))
    mem_address = next_pc_1 + next_pc_2;
  else
    mem_address = inst->IR.zero_ext(7) << 1;

  //Shifter or ALU generation
  bits16 alu_shifter_output;
  if(micro_seq.Get_ALU_RESULTMUX(inst->AGEX_CS)) //ALU operation
  {
    //determine the second input to the ALU
    bits16 input2;
    if(micro_seq.Get_SR2MUX(inst->AGEX_CS))
        input2 = inst->IR.sign_ext(4);
    else
        input2 = inst->SR2;

    //perfom the operation based on the micro code of the current instruction
    auto aluk = micro_seq.Get_ALUK(inst->AGEX_CS);
    switch(aluk.to_num())
    {
      case 0:
        alu_shifter_output = inst->SR1 + input2;
        break;
      case 1:
        alu_shifter_output = inst->SR1 & input2;
        break;
      case 2:
        alu_shifter_output = inst->SR1 ^ input2;
        break;
      case 3:
        alu_shifter_output = input2;
        break;
      default:
        assert(true);
    }
  }
  else //Shift Operation
  {
    //shif instruction
    //       [15       12|11     9|8      6| 5| 4| 3  2  1  0]
    //  LSHF [ 1, 1, 0, 1|   DR   |   SR   | 0| 0|   amount  ]
    //  RSHF [ 1, 1, 0, 1|   DR   |   SR   | 0| 1|   amount  ]
    // RSHFA [ 1, 1, 0, 1|   DR   |   SR   | 1| 1|   amount  ]
    //
    auto shif_mux = inst->IR.range<5,4>();
    auto shift_amount = inst->IR.range<3,0>();
    switch(shif_mux.to_num())
    {
      case 0: //LSHF
        alu_shifter_output = inst->SR1 << shift_amount.to_num();
        break;
      case 1: //RSHF
        alu_shifter_output = inst->SR1 >> shift_amount.to_num();
        break;
      case 3: //RSHFA
      {
        //arithmetic right shift, will preserve the MSB when shifting
        auto sr_15 = inst->SR1[15];
        alu_shifter_output = inst->SR1 >> shift_amount.to_num();
        for(auto i = 0; i < shift_amount.to_num(); ++i)
          alu_shifter_output[15-i] = sr_15;
        break;
      }
      default:
        assert(true);
    }
  }

  //set signals needed for previous stage
  agex_sig.agex_drid = inst->DRID;
  agex_sig.v_agex_ld_cc = agex_latch.V && micro_seq.Get_AGEX_LD_CC(inst->AGEX_CS);
  agex_sig.v_agex_ld_reg = agex_latch.V && micro_seq.Get_AGEX_LD_REG(inst->AGEX_CS);
  stall_sig.v_agex_br_stall = agex_latch.V && micro_seq.Get_AGEX_BR_STALL(inst->AGEX_CS);

  //Stall check
  auto LD_MEM = (stall_sig.mem_stall) ? 0 : 1;

  if (LD_MEM)
  {
    /* Propagate control signals from agex_sigs.CS latch to memory_sigs.CS latch. */
    inst->MEM_CS.range<10,0>() = inst->AGEX_CS.range<19,9>();
    
    // Propagate instruction object and V bit
    memory_latch.instruction = inst;
    memory_latch.V = agex_latch.V;
    inst->ADDRESS = mem_address;
    inst->ALU_RESULT = alu_shifter_output;
  } else {
    // mem_stall: Keep instruction in MEM by writing current MEM instruction to NEW_PS MEM latch
    auto & current_mem_latch = latch(MEMORY, PS);
    memory_latch.instruction = current_mem_latch.instruction;
    memory_latch.MEM_CS = current_mem_latch.MEM_CS;
    memory_latch.V = current_mem_latch.V;
  }
}

/************************* DE_stage() *************************/
void PipeLine::DE_stage()
{
  SetStage(DECODE);
  auto & cpu_state = simulator().state();
  auto & de_sig = cpu_state.DecodeSignals();
  auto & stall = cpu_state.Stall();
  auto & micro_sequencer = simulator().microsequencer();
  auto & decode_latch = latch(DECODE,PS);
  auto inst = decode_latch.instruction;
  auto & agex_latch = latch(AGEX,NEW_PS);
  bitfield<6> CONTROL_STORE_ADDRESS;

  if (!inst) {
    // No instruction in DECODE - this shouldn't happen, but handle gracefully
    return;
  }
  
  inst->current_stage = "D";
  
  //get micro code state
  CONTROL_STORE_ADDRESS.range<5,1>() = inst->IR.range<15,11>();
  CONTROL_STORE_ADDRESS[0] = inst->IR[5];
  de_sig.de_ucode = micro_sequencer.GetMicroCodeAt(CONTROL_STORE_ADDRESS.to_num());

  //The instruction in the decode_sigs stage also reads the register file and the condition codes.
  //The register file has two read ports: one for SR1 and one for SR2. decode_sigs.IR[8:6] are used
  //to address the register file to read SR1. Either decode_sigs.IR[11:9] or decode_sigs.IR[2:0] are used to
  //address the register file to read SR2. decode_sigs.IR[13] is used to select between
  //decode_sigs.IR[11:9] or decode_sigs.IR[2:0]. 2At the end of the clock cycle, the SR1 value from the
  //register file is latched into the agex_sigs.SR1 latch, and the SR2 value is latched into the
  //agex_sigs.SR2 latch.
  ProcessRegisterFile(inst->IR);

  //The value of the condition codes is latched into the 3-bit agex_sigs.CC latch.
  //Condition code N is stored in agex_sigs.CC[2], Z is stored in agex_sigs.CC[1], and P is stored in
  //agex_sigs.CC[0]. Note that the condition codes and register file are read and the values
  //obtained are latched regardless of whether an instruction needs these values.
  de_sig.de_cc = cpu_state.GetNZP();

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
    // Propagate control signals to instruction
    inst->AGEX_CS.range<19,0>() = de_sig.de_ucode.range<22,3>();

    /*agex_sigs Valid: valid if no stall or bubbles were detected*/
    bool agex_valid = (!stall.dep_stall) && (decode_latch.V);
    
    // Always propagate instruction object and V bit
    agex_latch.instruction = inst;
    agex_latch.V = agex_valid;
    inst->SR1 = de_sig.de_sr1_data;
    inst->SR2 = de_sig.de_sr2_data;
    inst->CC = de_sig.de_cc;
    
    if(micro_sequencer.Get_DRMUX(de_sig.de_ucode))
      inst->DRID = 0x7;
    else
      inst->DRID = inst->IR.range<11,9>();
  } else {
    // mem_stall: Keep instruction in AGEX by writing current AGEX instruction to NEW_PS
    auto & current_agex_latch = latch(AGEX, PS);
    agex_latch.instruction = current_agex_latch.instruction;
    agex_latch.V = current_agex_latch.V;
  }
}

/************************* FETCH_stage() *************************/
void PipeLine::FETCH_stage()
{
  SetStage(FETCH);
  auto & cpu_state = simulator().state();
  auto & memory_sig = simulator().state().MemSignals();
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
  switch(memory_sig.mem_pc_mux.to_num())
  {
    case 0:
      new_pc = de_npc;
      break;
    case 1:
      new_pc = memory_sig.target_pc;
      break;
    case 2:
      new_pc = memory_sig.trap_pc;
      break;
    default:
      assert(1); //TODO: should not happen. add an exception?
      break;
  }

  //if there are no stalls or control instructions in the pipeline,
  //the PC should be incremented by 2.
  auto load_pc = !IsStallDetected();
  if(load_pc)
  {
    cpu_state.SetProgramCounter(new_pc);
  }

  //do not latch the decode_sigs in case there is a data stall or dependency stall
  auto ld_de = (stall.dep_stall || stall.mem_stall) ? 0 : 1;
  if(ld_de)
  {
    //decode_sigs.valid is 0 if stall was detected or a branch
    //was not taken. Ohterwise, stage is good to go
    bool decode_valid = (!load_pc || stall.v_mem_br_stall) ? 0 : 1;
    
    // Always create instruction object - latch V bit indicates if it's valid or a bubble
    auto new_instr = Instruction::Create(simulator(), instruction);
    new_instr->PC = cpu_state.GetProgramCounter();
    new_instr->IR = instruction;
    new_instr->NPC = de_npc;
    new_instr->fetch_cycle = simulator().GetCycles();
    new_instr->current_stage = "F";
    decode_latch.instruction = new_instr;
    decode_latch.V = decode_valid;
  } else {
    // dep_stall or mem_stall: Keep instruction in DECODE by copying from PS
    auto & current_decode_latch = latch(DECODE, PS);
    decode_latch.instruction = current_decode_latch.instruction;
    decode_latch.V = current_decode_latch.V;
  }
}

void PipeLine::DumpHistory()
{
  // This function is called ONCE at the end of the simulation
  if (simulator().dump_file != nullptr)
    idump(simulator().dump_file);
}