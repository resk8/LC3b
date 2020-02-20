#include <map>

/***************************************************************/
/* Definition of the memory size.                              */
/***************************************************************/
#define WORDS_IN_MEM    0x08000 

class Simulator;
class MainMemory
{
  public:
  MainMemory(Simulator & instance);
  ~MainMemory(){}

  Simulator & simulator() { return _simulator; }

  void init_memory();
  uint_16 GetLowerByteAt(uint_16 address) const;
  void SetLowerByteAt(uint_16 address, uint_16 val);
  uint_16 GetUpperByteAt(uint_16 address) const;
  void SetUpperByteAt(uint_16 address, uint_16 val);
  void dcache_access(uint_16 dcache_addr, uint_16 *read_word, uint_16 write_word, uint_16 *dcache_r, uint_16 mem_w0, uint_16 mem_w1);
  void icache_access(uint_16 icache_addr, uint_16 *read_word, uint_16 *icache_r);
  void mdump(FILE * dumpsim_file, uint_16 start, uint_16 stop);

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
  std::vector<std::vector<uint_16>> MEMORY;
};