# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
mkdir -p build && cd build
cmake -G 'Unix Makefiles' -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make
```

Binary: `build/source/lC3b`

## Run

```bash
./build/source/lC3b <microcode_file> <program_file>
# Example:
cd doc/test && ../../build/source/lC3b ucode example.obj
```

Interactive commands: `go`, `run n`, `rdump`, `idump`, `mdump low high`, `cdump`, `quit`.

## Assemble

```bash
python3 doc/test/lc3b_assembler.py <input.asm> [output.obj]
```

## Architecture Overview

The simulator is a cycle-accurate 5-stage pipelined LC-3b processor. The central class is `Simulator` (owns all subsystems); `PipeLine` drives each clock cycle.

**Subsystem ownership in `Simulator`:**
- `PipeLine` — 5-stage pipeline logic (F → DE → AGEX → MEM → SR)
- `State` — architectural registers (R0–R7), PSR, NZP condition codes, USP/SSP
- `MainMemory` — 128KB byte-addressable memory + I-cache/D-cache simulation
- `MicroSequencer` — loads `ucode` file; maps opcodes → 25-bit control words
- `InterruptController` — priority queue of pending interrupts; drives pipeline drain and vector table lookup

**Pipeline latches:** Each stage has a `Latch` (defined in [include/Latch.h](include/Latch.h)) that holds the in-flight `Instruction` object. Latches are double-buffered (`current` / `next`) and swapped each cycle via `PropagatePipeLine()`.

**Instruction object** (`include/instruction.h`): A shared pointer carried through all stages. Holds IR, PC, NPC, SR1/SR2 data, ALU_RESULT, ADDRESS, AGEX_CS and MEM_CS control signal subsets, and RTI state-machine state. Created by `Instruction::Create()` (factory) in the Fetch stage.

**Control store bits** (`include/LC3b.h`, `enum CS_BITS`): The microcode word is indexed by bits 15–12 of the instruction. Key signals: `SR1_NEEDED`, `SR2_NEEDED` (stall detection), `ALUK[1:0]` (ALU op), `DCACHE_EN`/`DCACHE_RW`/`DATA_SIZE` (memory), `LD_REG`/`LD_CC`/`LD_PSR` (writeback), `BR_OP`/`UNCOND_OP`/`BR_STALL`, `TRAP_OP`, `RTI_OP`.

**Hazard detection** (`PipeLine::IsStallDetected()`): Checks RAW dependencies by comparing SR1/SR2 of the DE-stage instruction against DRID of in-flight AGEX/MEM/SR instructions that have `LD_REG` set. Stall inserts a bubble (NOP latch) and keeps fetch/DE frozen.

**Branch resolution** (`PipeLine::IsBranchTaken()`): Branches resolve in MEM stage; pipeline squashes younger instructions via `MEMORY_stage_Entry::target_pc`.

**Interrupt handling:** `InterruptController` raises an interrupt when priority > PSR[10:8]. `PipeLine` detects this, drains the pipeline (freeze fetch until all stages empty), then performs: SSP ← R6, R6 ← SSP, push PC then PSR to supervisor stack, load handler PC from vector table (0x0100 + vector×2).

**RTI:** Two-cycle state machine inside `Instruction` for RTI: pop saved PC then PSR from supervisor stack, swap USP/SSP, restore privilege and priority in PSR.

**Timing diagram** (`idump`): Written to `dumpsim.txt`. Each row is one instruction; columns are cycle numbers; cells are stage abbreviations (F/D/E/M/S) with `*` for stall.

## Key Files

| File | Role |
|---|---|
| [source/PipeLine.cpp](source/PipeLine.cpp) | All 5 stage implementations + hazard/branch logic |
| [include/LC3b.h](include/LC3b.h) | `CS_BITS` enum, type aliases (`bits16`, `bits3`, etc.) |
| [include/instruction.h](include/instruction.h) | `Instruction` class, `ExceptionType` enum, RTI state |
| [include/Latch.h](include/Latch.h) | Per-stage latch structure |
| [include/State.h](include/State.h) | `DE_Stage_Entry`, `AGEX_Stage_Entry`, `MEMORY_stage_Entry`, `STORE_Stage_Entry` forwarding structs |
| [source/InterruptController.cpp](source/InterruptController.cpp) | Interrupt priority queue and state machine |
| [doc/test/ucode](doc/test/ucode) | Microcode ROM (one 25-bit control word per opcode) |
| [doc/test/lc3b_assembler.py](doc/test/lc3b_assembler.py) | Two-pass assembler (`.asm` → `.obj`) |
