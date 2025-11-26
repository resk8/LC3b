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
  // Helper macro to print to both terminal and file, removing redundancy.
#define PRINT_AND_DUMP(...) \
    do { \
        printf(__VA_ARGS__); \
        if (dumpsim_file) { fprintf(dumpsim_file, __VA_ARGS__); } \
    } while (0)

  State & cpu_state = simulator().state();

  PRINT_AND_DUMP("\nCurrent architectural state :\n");
  PRINT_AND_DUMP("-------------------------------------\n");
  PRINT_AND_DUMP("Cycle Count     : %d\n", simulator().GetCycles());
  PRINT_AND_DUMP("PC                : 0x%04x\n", cpu_state.GetProgramCounter().to_num());
  PRINT_AND_DUMP("CCs: N = %d  Z = %d  P = %d\n", cpu_state.GetNBit(), cpu_state.GetZBit(), cpu_state.GetPBit());
  PRINT_AND_DUMP("Registers:\n");
  for (auto k = 0; k < LC3b_REGS; k++)
  {
    PRINT_AND_DUMP("R%d: 0x%04x\n", k, cpu_state.GetRegisterData(k).to_num());
  }
  PRINT_AND_DUMP("\n");

  // Get references to all latches
  const auto & decode = latch(DECODE,PS);
  const auto & agex = latch(AGEX,PS);
  const auto & memory = latch(MEMORY,PS);
  const auto & store = latch(STORE,PS);

  // --- table formatting ---

  const int COL_WIDTH = 24;

  // Helper to create a padded string for a table cell
  auto format_cell = [&](const std::string& content) -> std::string {
    std::string padded = "| " + content;
    if (padded.length() > COL_WIDTH) {
      padded = padded.substr(0, COL_WIDTH - 4) + "... ";
    } else {
      padded.append(COL_WIDTH - padded.length(), ' ');
    }
    return padded;
  };

  // Helper to format a key-value pair
  auto kv_format = [](const std::string& key, const std::string& val) -> std::string {
      return key + ": " + val;
  };

  // Helper to format hex values
  auto hex_str = [](uint16_t val) {
    char buf[10];
    snprintf(buf, sizeof(buf), "0x%04x", Low16bits(val));
    return std::string(buf);
  };

  // Table Header
  std::string header = format_cell("STALLS") + format_cell("DECODE") + format_cell("AGEX") + format_cell("MEMORY") + format_cell("STORE") + "|";
  std::string separator(header.length(), '-');
  PRINT_AND_DUMP("%s\n", separator.c_str());
  PRINT_AND_DUMP("%s\n", header.c_str());
  PRINT_AND_DUMP("%s\n", separator.c_str());

  Disassembler disassembler;

  // Instruction
  PRINT_AND_DUMP("%s%s%s%s%s|\n",
    format_cell(kv_format("ICACHE_R", std::to_string(cpu_state.Stall().icache_r))).c_str(),
    format_cell(kv_format("Inst", (decode.V) ? disassembler.disassemble(decode.IR) : "---")).c_str(),
    format_cell(kv_format("Inst", (agex.V) ? disassembler.disassemble(agex.IR) : "---")).c_str(),
    format_cell(kv_format("Inst", (memory.V) ? disassembler.disassemble(memory.IR) : "---")).c_str(),
    format_cell(kv_format("Inst", (store.V) ? disassembler.disassemble(store.IR) : "---")).c_str());

  PRINT_AND_DUMP("%s%s%s%s%s|\n",
    format_cell(kv_format("DEP_STALL", std::to_string(cpu_state.Stall().dep_stall))).c_str(),
    format_cell(kv_format("V", std::to_string(decode.V))).c_str(),
    format_cell(kv_format("V", std::to_string(agex.V))).c_str(),
    format_cell(kv_format("V", std::to_string(memory.V))).c_str(),
    format_cell(kv_format("V", std::to_string(store.V))).c_str());

  PRINT_AND_DUMP("%s%s%s%s%s|\n",
    format_cell(kv_format("V_DE_BR_STALL", std::to_string(cpu_state.Stall().v_de_br_stall))).c_str(),
    format_cell(kv_format("NPC", (decode.V) ? hex_str(decode.NPC.to_num()) : "---")).c_str(),
    format_cell(kv_format("NPC", (agex.V) ? hex_str(agex.NPC.to_num()) : "---")).c_str(),
    format_cell(kv_format("NPC", (memory.V) ? hex_str(memory.NPC.to_num()) : "---")).c_str(),
    format_cell(kv_format("NPC", (store.V) ? hex_str(store.NPC.to_num()) : "---")).c_str());

  PRINT_AND_DUMP("%s%s%s%s%s|\n",
    format_cell(kv_format("V_AGEX_BR_STALL", std::to_string(cpu_state.Stall().v_agex_br_stall))).c_str(),
    format_cell(kv_format("IR", (decode.V) ? hex_str(decode.IR.to_num()) : "---")).c_str(),
    format_cell(kv_format("IR", (agex.V) ? hex_str(agex.IR.to_num()) : "---")).c_str(),
    format_cell(kv_format("IR", (memory.V) ? hex_str(memory.IR.to_num()) : "---")).c_str(),
    format_cell(kv_format("IR", (store.V) ? hex_str(store.IR.to_num()) : "---")).c_str());

  PRINT_AND_DUMP("%s%s%s%s%s|\n",
    format_cell(kv_format("MEM_STALL", std::to_string(cpu_state.Stall().mem_stall))).c_str(),
    format_cell("").c_str(),
    format_cell(kv_format("SR1", (agex.V) ? hex_str(agex.SR1.to_num()) : "---")).c_str(),
    format_cell(kv_format("ALU_RESULT", (memory.V) ? hex_str(memory.ALU_RESULT.to_num()) : "---")).c_str(),
    format_cell(kv_format("DATA", (store.V) ? hex_str(store.DATA.to_num()) : "---")).c_str());

  PRINT_AND_DUMP("%s%s%s%s%s|\n",
    format_cell(kv_format("V_MEM_BR_STALL", std::to_string(cpu_state.Stall().v_mem_br_stall))).c_str(),
    format_cell("").c_str(),
    format_cell(kv_format("SR2", (agex.V) ? hex_str(agex.SR2.to_num()) : "---")).c_str(),
    format_cell(kv_format("ADDRESS", (memory.V) ? hex_str(memory.ADDRESS.to_num()) : "---")).c_str(),
    format_cell(kv_format("ALU_RESULT", (store.V) ? hex_str(store.ALU_RESULT.to_num()) : "---")).c_str());

  PRINT_AND_DUMP("%s\n", separator.c_str());

  fflush(dumpsim_file);

  // Undefine the macro to avoid polluting other functions
#undef PRINT_AND_DUMP
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
    throw std::runtime_error("Invalid stage for latch access"); // Should not happen
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
  if(de_latch.V)
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
  MoveLatch(NEW_PS,PS);
  SR_stage();
  MEM_stage();
  AGEX_stage();
  DE_stage();
  FETCH_stage();
  idump(simulator().dump_file);
  MoveLatch(PS,NEW_PS);
}

/************************* SR_stage() *************************/
void PipeLine::SR_stage()
{
  SetStage(STORE);
  auto & micro_sequencer =  simulator().microsequencer();
  auto & sr_sig = simulator().state().SrSignals();
  auto & store_latch = latch(STORE,PS);
  /* You are given the code for SR_stage to get you started. Look at
     the figure for store_signals stage to see how this code is implemented. */

  switch (micro_sequencer.Get_DR_VALUEMUX(store_latch.SR_CS).to_num())
  {
  case 0:
    sr_sig.sr_reg_data = store_latch.ADDRESS;
    break;
  case 1:
    sr_sig.sr_reg_data = store_latch.DATA;
    break;
  case 2:
    sr_sig.sr_reg_data = store_latch.NPC;
    break;
  case 3:
    sr_sig.sr_reg_data = store_latch.ALU_RESULT;
    break;
  }

  sr_sig.sr_drid = store_latch.DRID;
  sr_sig.v_sr_ld_reg = micro_sequencer.Get_SR_LD_REG(store_latch.SR_CS) & store_latch.V;
  sr_sig.v_sr_ld_cc = micro_sequencer.Get_SR_LD_CC(store_latch.SR_CS) & store_latch.V ;

  /* CC LOGIC  */
  sr_sig.sr_n = sr_sig.sr_reg_data[15];
  sr_sig.sr_z = ((sr_sig.sr_reg_data.to_num() == 0) ? 1 : 0);
  sr_sig.sr_p = ((!sr_sig.sr_n) && (!sr_sig.sr_z));
}

/************************* MEM_stage() *************************/
void PipeLine::MEM_stage()
{
  SetStage(MEMORY);
  auto & store_latch = latch(STORE,NEW_PS);
  auto & memory_latch = latch(MEMORY,PS);
  auto & main_memory = simulator().memory();
  auto & memory_sig = simulator().state().MemSignals();
  auto & stall_sig = simulator().state().Stall();
  auto & micro_seq = simulator().microsequencer();

  //access aligment logic
  auto alignment_needed = memory_latch.ADDRESS[0];
  auto data_size = micro_seq.Get_DATA_SIZE(memory_latch.MEM_CS);
  bits16 MDR_IN;
  if(!data_size) //byte access
  {
    bits8 alu_result = memory_latch.ALU_RESULT.range<7,0>();
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
    MDR_IN = memory_latch.ALU_RESULT;

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
  auto cache_en = micro_seq.Get_DCACHE_EN(memory_latch.MEM_CS) && memory_latch.V;
  if(cache_en)
    main_memory.dcache_access(memory_latch.ADDRESS, MDR_OUT, MDR_IN, data_cache_r, we_low, we_high);
  else
    data_cache_r = true; //no stall since memory was not even accessed

  //memory signals for fetch stage
  memory_sig.target_pc = memory_latch.ADDRESS;
  memory_sig.mem_drid = memory_latch.DRID;
  stall_sig.mem_stall = cache_en && (!data_cache_r);

  //Branch Logic
  auto memory_v = memory_latch.V;
  if(memory_v)
  {
    auto is_branch_op = micro_seq.Get_BR_OP(memory_latch.MEM_CS);
    auto is_trap_op = micro_seq.Get_TRAP_OP(memory_latch.MEM_CS);
    if(is_branch_op)
    {
      bits3 br_intr_nzp = memory_latch.IR.range<11,9>();
      bits3 cpu_nzp = memory_latch.CC;
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
  memory_sig.v_mem_ld_cc = memory_v && micro_seq.Get_MEM_LD_CC(memory_latch.MEM_CS);
  memory_sig.v_mem_ld_reg = memory_v && micro_seq.Get_MEM_LD_REG(memory_latch.MEM_CS);
  stall_sig.v_mem_br_stall = memory_v && micro_seq.Get_MEM_BR_STALL(memory_latch.MEM_CS);

  //load SR latch
  store_latch.ADDRESS = memory_latch.ADDRESS;
  store_latch.DATA = memory_sig.trap_pc;
  store_latch.NPC = memory_latch.NPC;
  store_latch.ALU_RESULT = memory_latch.ALU_RESULT;
  store_latch.IR = memory_latch.IR;
  store_latch.DRID = memory_latch.DRID;

  /* The code below propagates the control signals from memory_sigs.CS latch
     to store_signals.CS latch. You still need to latch other values into the
     other store_signals latches. */
  store_latch.SR_CS = memory_latch.MEM_CS.range<10,7>();
  store_latch.V = memory_v && (!stall_sig.mem_stall);
}

/************************* AGEX_stage() *************************/
void PipeLine::AGEX_stage()
{
  SetStage(AGEX);
  auto & memory_latch = latch(MEMORY,NEW_PS);
  auto & agex_latch = latch(AGEX,PS);
  auto & agex_sig = simulator().state().AgexSignals();
  auto & stall_sig = simulator().state().Stall();
  auto & micro_seq = simulator().microsequencer();

  /* your code for agex_sigs stage goes here */
  // First program counter mux
  bits16 next_pc_1;
  if(micro_seq.Get_ADDR1MUX(agex_latch.AGEX_CS))
    next_pc_1 = agex_latch.SR1;
  else
    next_pc_1 = agex_latch.NPC;

  // second program counter mux
  // TODO: this might be broken because of the bit access to non_const overload
  bits16 next_pc_2;
  auto agex_addr2mux = micro_seq.Get_ADDR2MUX(agex_latch.AGEX_CS);
  switch (agex_addr2mux.to_num())
  {
  case 0:
    next_pc_2 = 0;
    break;
  case 1:
    next_pc_2 = agex_latch.IR.sign_ext(5);
    break;
  case 2:
    next_pc_2 = agex_latch.IR.sign_ext(8);
    break;
  case 3:
    next_pc_2 = agex_latch.IR.sign_ext(10);
    break;
  default:
    assert(true); //TODO: should not happen. add an exception?
  }

  //check if lshf1 signal is 1
  if(micro_seq.Get_LSHF1(agex_latch.AGEX_CS))
    next_pc_2 = next_pc_2 << 1;

  //generate next instruction address
  bits16 mem_address;
  if(micro_seq.Get_ADDRESSMUX(agex_latch.AGEX_CS))
    mem_address = next_pc_1 + next_pc_2;
  else
    mem_address = agex_latch.IR.zero_ext(7) << 1;

  //Shifter or ALU generation
  bits16 alu_shifter_output;
  if(micro_seq.Get_ALU_RESULTMUX(agex_latch.AGEX_CS)) //ALU operation
  {
    //determine the second input to the ALU
    bits16 input2;
    if(micro_seq.Get_SR2MUX(agex_latch.AGEX_CS))
        input2 = agex_latch.IR.sign_ext(4);
    else
        input2 = agex_latch.SR2;

    //perfom the operation based on the micro code of the current instruction
    auto aluk = micro_seq.Get_ALUK(agex_latch.AGEX_CS);
    switch(aluk.to_num())
    {
      case 0:
        alu_shifter_output = agex_latch.SR1 + input2;
        break;
      case 1:
        alu_shifter_output = agex_latch.SR1 & input2;
        break;
      case 2:
        alu_shifter_output = agex_latch.SR1 ^ input2;
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
    auto shif_mux = agex_latch.IR.range<5,4>();
    auto shift_amount = agex_latch.IR.range<3,0>();
    switch(shif_mux.to_num())
    {
      case 0: //LSHF
        alu_shifter_output = agex_latch.SR1 << shift_amount.to_num();
        break;
      case 1: //RSHF
        alu_shifter_output = agex_latch.SR1 >> shift_amount.to_num();
        break;
      case 3: //RSHFA
      {
        //arithmetic right shift, will preserve the MSB when shifting
        auto sr_15 = agex_latch.SR1[15];
        alu_shifter_output = agex_latch.SR1 >> shift_amount.to_num();
        for(auto i = 0; i < shift_amount.to_num(); ++i)
          alu_shifter_output[15-i] = sr_15;
        break;
      }
      default:
        assert(true);
    }
  }

  //set signals needed for previous stage
  agex_sig.agex_drid = agex_latch.DRID;
  agex_sig.v_agex_ld_cc = agex_latch.V && micro_seq.Get_AGEX_LD_CC(agex_latch.AGEX_CS);
  agex_sig.v_agex_ld_reg = agex_latch.V && micro_seq.Get_AGEX_LD_REG(agex_latch.AGEX_CS);
  stall_sig.v_agex_br_stall = agex_latch.V && micro_seq.Get_AGEX_BR_STALL(agex_latch.AGEX_CS);

  //Stall check
  auto LD_MEM = (stall_sig.mem_stall) ? 0 : 1;

  if (LD_MEM)
  {
    /* Your code for latching into memory_sigs latches goes here */
    memory_latch.ADDRESS = mem_address;
    memory_latch.NPC = agex_latch.NPC;
    memory_latch.CC = agex_latch.CC;
    memory_latch.ALU_RESULT = alu_shifter_output;
    memory_latch.IR = agex_latch.IR;
    memory_latch.DRID = agex_latch.DRID;
    memory_latch.V = agex_latch.V;

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
  auto & de_sig = cpu_state.DecodeSignals();
  auto & stall = cpu_state.Stall();
  auto & micro_sequencer = simulator().microsequencer();
  auto & decode_latch = latch(DECODE,PS);
  auto & agex_latch = latch(AGEX,NEW_PS);
  bitfield<6> CONTROL_STORE_ADDRESS;

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
    agex_latch.NPC = decode_latch.NPC;
    agex_latch.IR = decode_latch.IR;
    agex_latch.SR1 = de_sig.de_sr1_data;
    agex_latch.SR2 = de_sig.de_sr2_data;
    agex_latch.CC = de_sig.de_cc;
    agex_latch.AGEX_CS.range<19,0>() = de_sig.de_ucode.range<22,3>();

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
    decode_latch.IR = instruction;
    decode_latch.NPC = de_npc;

    //decode_sigs.valid is 0 if stall was detected or a branch
    //was not taken. Ohterwise, stage is good to go
    decode_latch.V = load_pc;
  }
}