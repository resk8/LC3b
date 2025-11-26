/***************************************************************/
/* MainMemory.h: LC-3b Main Memory Class Header File           */
/***************************************************************/
#pragma once

#include <vector>
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
  bits8 GetLowerByteAt(const bits16 & address) const;
  void SetLowerByteAt(const bits16 & address, bits8 val);
  bits8 GetUpperByteAt(const bits16 & address) const;
  void SetUpperByteAt(const bits16 & address, bits8 val);
  void dcache_access(const bits16 & dcache_addr, bits16 & read_word, const bits16 & write_word, bool & dcache_r, bool mem_w0, bool mem_w1);
  void icache_access(const bits16 & icache_addr, bits16 & read_word, bool & icache_r);
  void mdump(FILE * dumpsim_file, const bits16 & start, const bits16 & stop);

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
  std::vector<std::vector<bits8>> MEMORY;
};