#include <map>
#ifdef __linux__ 
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

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
  uint8_t GetLowerByteAt(uint16_t address) const;
  void SetLowerByteAt(uint16_t address, uint8_t val);
  uint8_t GetUpperByteAt(uint16_t address) const;
  void SetUpperByteAt(uint16_t address, uint8_t val);
  void dcache_access(uint16_t dcache_addr, uint16_t & read_word, uint16_t write_word, bool & dcache_r, uint16_t mem_w0, uint16_t mem_w1);
  void icache_access(uint16_t icache_addr, uint16_t & read_word, bool & icache_r);
  void mdump(FILE * dumpsim_file, uint16_t start, uint16_t stop);

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
  std::vector<std::vector<uint8_t>> MEMORY;
};