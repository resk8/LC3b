#include<memory>
#include<vector>
#ifdef __linux__ 
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

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
  void cycle();
  void run(int num_cycles);
  void go();
  void idump(FILE * dumpsim_file);
  void get_command(FILE * dumpsim_file);  
  void load_program(char *program_filename);
  void initialize(char *ucode_filename, char *program_filename, uint16_t num_prog_files);
  int  GetCycles() const { return CYCLE_COUNT; }
  bool GetRunBit() const { return RUN_BIT; }

  private:
  std::shared_ptr<MainMemory> CpuMemory;
  std::shared_ptr<MicroSequencer> CpuMicroSequencer;
  std::shared_ptr<PipeLine> CpuPipeline;
  std::shared_ptr<State> CpuState;


  /* A cycle counter */
  int CYCLE_COUNT;

  /* simulator signal */
  bool RUN_BIT;
};
