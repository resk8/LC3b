#include <map>

/***************************************************************/
/* Definition of the memory size.                              */
/***************************************************************/
#define WORDS_IN_MEM    0x08000 

class Simulator;
class MainMemory
{
  public:
  MainMemory(Simulator & instance) : _simulator(instance) {}
  ~MainMemory(){}

  Simulator & simulator() { return _simulator; }

  void init_memory();
  int GetLowerByteAt(int address) const;
  void SetLowerByteAt(int address, int val);
  int GetUpperByteAt(int address) const;
  void SetUpperByteAt(int address, int val);
  void dcache_access(int dcache_addr, int *read_word, int write_word, int *dcache_r, int mem_w0, int mem_w1);
  void icache_access(int icache_addr, int *read_word, int *icache_r);
  void mdump(FILE * dumpsim_file, int start, int stop);

  private:
  Simulator & _simulator;
  /***************************************************************/
  /* Main memory.                                                */
  /***************************************************************/
  /* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */
  std::map<int,std::map<int,int>> MEMORY;
};