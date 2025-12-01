[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/resk8/LC3b) 

# LC-3b Pipelined Simulator

A cycle-accurate simulator for the LC-3b (Little Computer 3, byte-addressable) ISA with a 5-stage pipeline implementation. This simulator provides detailed visualization of instruction flow through the pipeline, including hazard detection and resolution.

## Table of Contents
- [Features](#features)
- [The 5-Stage Pipeline](#the-5-stage-pipeline)
- [Building the Project](#building-the-project)
- [Running the Simulator](#running-the-simulator)
- [Pipeline Timing Diagram](#pipeline-timing-diagram)
- [Project Structure](#project-structure)
- [Understanding the Microcode](#understanding-the-microcode)

## Features

- **5-Stage Pipeline**: Implements Fetch, Decode, Execute, Memory, and Store stages
- **Microcoded Control**: Control logic driven by a microcode file ([`doc/test/ucode`](doc/test/ucode))
- **Hazard Detection & Resolution**:
  - **Data Hazards**: Detects Read-After-Write (RAW) dependencies and inserts pipeline stalls
  - **Control Hazards**: Handles branch instructions with stalls until resolution in Memory stage
- **Cache Simulation**: Models instruction and data caches with variable latency
- **Detailed Timing Diagram**: Generates cycle-by-cycle visualization in `dumpsim.txt`
- **Instruction Disassembly**: Human-readable instruction format in output

## The 5-Stage Pipeline

The simulator models a classic RISC pipeline with the following stages:

| Stage    | Name    | Description                                              |
|----------|---------|----------------------------------------------------------|
| **F**    | Fetch   | Fetches instruction from I-Cache using PC                |
| **DE**   | Decode  | Decodes instruction, reads registers, detects dependencies |
| **AGEX** | Execute | Performs ALU operations or address calculations          |
| **MEM**  | Memory  | Accesses D-Cache for loads/stores, resolves branches    |
| **SR**   | Store   | Writes results back to register file                     |

### Pipeline Implementation Details

- **Register File**: 8 general-purpose registers (R0-R7)
- **Condition Codes**: N (Negative), Z (Zero), P (Positive)
- **Memory**: 32K words (16-bit word addressable)
- **Data Path**: Implements forwarding paths (configurable via microcode)

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C++14 compatible compiler (GCC, Clang, or MSVC)

### Build Instructions

```bash
# Create and enter build directory
mkdir -p build && cd build

# Generate build files
cmake -G 'Unix Makefiles' -D CMAKE_EXPORT_COMPILE_COMMANDS=1 ..

# Build the simulator
make

# Return to project root
cd ..
```

The executable will be located at `build/source/lC3b`.

## Running the Simulator

### Basic Usage

```bash
./build/source/lC3b <microcode_file> <program_file>
```

### Example

```bash
cd doc/test
../../build/source/lC3b ucode example.obj
```

### Interactive Commands

Once running, the simulator provides an interactive shell:

| Command           | Description                                      |
|-------------------|--------------------------------------------------|
| `go`              | Run program until completion (PC = 0x0000)       |
| `run n`           | Execute program for `n` cycles                   |
| `mdump low high`  | Dump memory from address `low` to `high`         |
| `rdump`           | Dump architectural state (registers, PC, CCs)    |
| `idump`           | Display pipeline timing diagram                  |
| `cdump`           | Dump control store (microcode)                   |
| `?`               | Display help menu                                |
| `quit`            | Exit simulator                                   |

## Pipeline Timing Diagram

The simulator's most powerful feature is its detailed timing diagram, saved to `dumpsim.txt`. This diagram shows the exact state of every instruction in every cycle.

### Diagram Format

```
PC      | Instruction                   | Mem Addr  | C0   | C1   | C2   | C3   | C4   | ...
--------+-------------------------------+-----------+------+------+------+------+------+----
0x3000  | LEA R0, #5                    |           | F    | D    | E    | M    | S    |
0x3002  | LDW R1, R0, #0                | 0x300a    |      | F    | D*   | D*   | E    | M
```

### Column Descriptions

- **PC**: Program Counter value (instruction address)
- **Instruction**: Disassembled assembly instruction
- **Mem Addr**: Memory address accessed (for LDW/STW/LDB/STB only)
- **Cn**: Cycle number columns showing pipeline stage

### Stage Indicators

| Symbol              | Meaning                                          |
|---------------------|--------------------------------------------------|
| `F`                 | Fetch stage                                      |
| `D`                 | Decode stage                                     |
| `E`                 | Execute stage                                    |
| `M`                 | Memory stage                                     |
| `S`                 | Store (writeback) stage                          |
| `D*`, `E*`, `M*`    | Stalled in that stage                            |
| (blank)             | Instruction not yet fetched or already retired   |

### Example: Simple Loop with Dependencies

Consider this LC-3b assembly program:

```assembly
.ORIG x3000
        LEA R0, DATA    ; Load address of DATA
        LDW R1, R0, #0  ; Load value from memory → R1
        ADD R2, R1, #5  ; R2 = R1 + 5 (depends on R1)
        ADD R3, R2, #3  ; R3 = R2 + 3 (depends on R2)
        STW R3, R0, #1  ; Store R3 to memory
        HALT
DATA:   .FILL x000A
```

**Generated Timing Diagram:**

```
PC      | Instruction                   | Mem Addr  | C0   | C1   | C2   | C3   | C4   | C5   | C6   | C7   | C8   | C9   | C10  | C11  | C12  |
--------+-------------------------------+-----------+------+------+------+------+------+------+------+------+------+------+------+------+------+
0x3000  | LEA R0, #5                    |           | F    | D    | E    | M    | S    |      |      |      |      |      |      |      |      |
0x3002  | LDW R1, R0, #0                | 0x300c    |      | F    | D    | D*   | E    | M    | S    |      |      |      |      |      |      |
0x3004  | ADD R2, R1, #5                |           |      |      | F    | D    | D*   | D*   | E    | M    | S    |      |      |      |      |
0x3006  | ADD R3, R2, #3                |           |      |      |      | F    | D    | D*   | D*   | D*   | E    | M    | S    |      |      |
0x3008  | STW R3, R0, #1                | 0x300e    |      |      |      |      | F    | D    | D*   | D*   | D*   | D*   | E    | M    | S    |
0x300a  | HALT                          |           |      |      |      |      |      | F    | D    | D*   | D*   | D*   | D*   | E    | M    |
```

### Analysis of the Example

**Cycle-by-Cycle Breakdown:**

1. **C0**: `LEA` enters Fetch
2. **C1**: `LEA` moves to Decode, `LDW` enters Fetch
3. **C2**: `LEA` in Execute, `LDW` in Decode, `ADD R2` enters Fetch
4. **C3**: 
   - `LEA` in Memory, `LDW` **stalled** in Decode (`D*`)
   - Reason: `LDW` needs R0, which `LEA` hasn't written yet
5. **C4**: 
   - `LEA` completes in Store, `LDW` proceeds to Execute
   - `ADD R2` **stalled** in Decode (`D*`)
   - Reason: `ADD R2` needs R1, which `LDW` is still loading
6. **C5-C6**: `LDW` continues through Memory and Store
7. **C7**: `ADD R2` finally proceeds to Execute after R1 is available
8. **C8**: `ADD R3` **stalled** waiting for R2 from previous `ADD`
9. **C9-C10**: Cascading stalls continue as dependencies resolve
10. **C11**: `STW` enters Execute with all dependencies resolved

**Key Observations:**

- **Data Dependency Stalls**: The `D*` markers show where instructions wait for register values
- **Memory Address Tracking**: The `Mem Addr` column shows `0x300c` for `LDW` and `0x300e` for `STW`
- **Pipeline Efficiency**: Total cycles = 12, vs. 6 instructions × 5 stages = 30 cycles (if fully serialized)
- **Stall Propagation**: Each dependency causes a "bubble" that propagates through the pipeline

## Project Structure

```
LC3b/
├── include/              # Header files
│   ├── BitField.h       # Template for arbitrary-width bit fields
│   ├── Disassembler.h   # Instruction disassembly
│   ├── instruction.h    # Instruction class definition
│   ├── Latch.h          # Pipeline latch structures
│   ├── LC3b.h           # ISA definitions and constants
│   ├── MainMemory.h     # Memory and cache simulation
│   ├── MicroSequencer.h # Control store management
│   ├── PipeLine.h       # Pipeline control logic
│   ├── Simulator.h      # Main simulator class
│   └── State.h          # Architectural state (registers, CCs)
├── source/               # Implementation files
│   ├── Disassembler.cpp
│   ├── instruction.cpp
│   ├── Latch.cpp
│   ├── LC3b.cpp         # Main entry point
│   ├── MainMemory.cpp
│   ├── MicroSequencer.cpp
│   ├── PipeLine.cpp     # Core pipeline simulation
│   ├── Simulator.cpp
│   └── State.cpp
├── doc/test/
│   ├── ucode            # Microcode control store ROM
│   └── dumpsim.txt      # Generated timing diagram output
└── build/                # CMake build directory
```

## Understanding the Microcode

The simulator uses a microcode ROM ([`doc/test/ucode`](doc/test/ucode)) to control pipeline behavior. Each instruction's opcode indexes into the control store to retrieve control signals.

**Example microcode entry for ADD (Register Mode):**

| Signal               | Value | Description                                    |
|----------------------|-------|------------------------------------------------|
| `SR1.NEEDED`         | 1     | Source register 1 is required                  |
| `SR2.NEEDED`         | 1     | Source register 2 is required                  |
| `DRMUX`              | 0     | Destination register selection                 |
| `ADDR1MUX`           | 0     | Address calculation mux 1                      |
| `ADDR2MUX`           | 0     | Address calculation mux 2                      |
| `LSHF1`              | 0     | Left shift by 1 bit                            |
| `ADDRESSMUX`         | 0     | Final address mux                              |
| `SR2MUX`             | 0     | Source register 2 / immediate mux              |
| `ALUK[1:0]`          | 00    | ALU operation: ADD                             |
| `ALU.RESULTMUX`      | 0     | ALU result selection                           |
| `BR.OP`              | 1     | Branch operation enable                        |
| `UNCOND`             | 0     | Unconditional branch                           |
| `TRAP`               | 0     | Trap instruction                               |
| `BR.STALL`           | 0     | Branch stall signal                            |
| `DCACHE.EN`          | 0     | Data cache enable (no memory access)           |
| `DCACHE.RW`          | 0     | Data cache read/write                          |
| `DATA.SIZE`          | 0     | Data size (word/byte)                          |
| `DR.VALUEMUX[1:0]`   | 00    | Destination register value source              |
| `LD.REG`             | 1     | **Load register file** (writeback enabled)     |
| `LD.CC`              | 1     | **Load condition codes** (update N, Z, P)      |

**Key Control Signals for ADD:**
- **Register Dependencies**: Both `SR1.NEEDED` and `SR2.NEEDED` are set, indicating this instruction requires two source registers
- **ALU Operation**: `ALUK = 00` selects addition
- **Writeback**: Both `LD.REG` and `LD.CC` are set to update the destination register and condition codes
- **No Memory Access**: `DCACHE.EN = 0` since ADD is a register-only operation

**Other Instruction Examples:**

| Instruction           | SR1 | SR2 | ALUK | DCACHE.EN | DCACHE.RW   | LD.REG | LD.CC |
|-----------------------|-----|-----|------|-----------|-------------|--------|-------|
| `ADD Rd, Rs, Rt`      | 1   | 1   | 00   | 0         | 0           | 1      | 1     |
| `LDR Rd, Rs, offset`  | 1   | 0   | 00   | 1         | 0 (Read)    | 1      | 1     |
| `STR Rs, Rd, offset`  | 1   | 1   | 00   | 1         | 1 (Write)   | 0      | 0     |
| `BR offset`           | 0   | 0   | 00   | 0         | 0           | 0      | 0     |

## Contributing

Contributions are welcome! Areas for improvement:
- Additional ISA instruction support
- More sophisticated branch prediction
- Cache hierarchy modeling
- Performance metrics collection

## License

This project is available for educational purposes. Please check with the original author for licensing details.

## References

- [LC-3b ISA Specification](https://www.cs.utexas.edu/users/fussell/courses/cs310h/lectures/Lecture_15-310h.pdf)
- [Pipelined Processor Design](https://www.cs.utexas.edu/~yale/cs352h/lectures/Pipelining.pdf)

---

**Note**: This simulator is designed for educational purposes to understand pipelined processor architecture and hazard handling mechanisms.