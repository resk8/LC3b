#include<memory>
//#include "Memory.h"
//#include "MicroSequencer.h"
//#include "PipeLine.h"
//#include "State.h"

class PipeLine;
class MainMemory;
class State;
class MicroSequencer;

class Simulator
{
  public:
  Simulator();
  ~Simulator(){}

  PipeLine & pipeline() {return *CpuPipeline; }
  MainMemory & memory() {return *CpuMemory; }
  State & state() {return *CpuState; }
  MicroSequencer & microsequencer() {return *CpuMicroSequencer; }
  
  void help();
  void print_CS(int *CS, int num);
  void cycle();
  void run(int num_cycles);
  void go();
  void idump(FILE * dumpsim_file);
  void get_command(FILE * dumpsim_file);  
  void load_program(char *program_filename);
  void initialize(char *ucode_filename, char *program_filename, int num_prog_files);
  int GetCycles() const { return CYCLE_COUNT; }
  int GetRunBit() const { return RUN_BIT; }

  private:
  std::shared_ptr<MainMemory> CpuMemory;
  std::shared_ptr<MicroSequencer> CpuMicroSequencer;
  std::shared_ptr<PipeLine> CpuPipeline;
  std::shared_ptr<State> CpuState;


  /* A cycle counter */
  int CYCLE_COUNT;

  /* simulator signal */
  int RUN_BIT;
};
