/***************************************************************/
/* Memory Implementaion                                        */
/***************************************************************/

#include <iostream>
#ifdef __linux__
    #include "../include/Simulator.h"
    #include "../include/MainMemory.h"
#else    
    #include "Simulator.h"
    #include "MainMemory.h"
#endif

/*
*
*/
MainMemory::MainMemory(Simulator & instance) : _simulator(instance) 
{
  MEMORY = std::vector<std::vector<bits8>>(WORDS_IN_MEM,std::vector<bits8>(2));
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void MainMemory::init_memory()
{
  for (auto i=0; i < WORDS_IN_MEM; i++) 
  {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/*
* //TODO
*/
bits8 MainMemory::GetLowerByteAt(bits16 address) const
{
  try
  {
    return MEMORY.at(address.to_num()).at(0);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Low byte read to invalid memory location: addr=0x%04hX\n",address.to_num());
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
* //TODO
*/
void MainMemory::SetLowerByteAt(bits16 address, bits8 val)
{
  try
  {
    MEMORY.at(address.to_num()).at(0) = val;
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Low byte write to invalid memory location: addr=0x%04hX\n",address.to_num());
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
* //TODO
*/
bits8 MainMemory::GetUpperByteAt(bits16 address) const
{
  try
  {
    return MEMORY.at(address.to_num()).at(1);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: High byte read to invalid memory location: addr=0x%04hX\n",address.to_num());
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
* //TODO
*/
void MainMemory::SetUpperByteAt(bits16 address, bits8 val)
{
  try
  {
    MEMORY.at(address.to_num()).at(1) = val;
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: High byte write to invalid memory location: addr=0x%04hXs\n",address.to_num());
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/***************************************************************/
/*                                                             */
/* dcache_access                                               */
/*                                                             */
/***************************************************************/
void MainMemory::dcache_access(bits16 dcache_addr, bits16 & read_word, bits16 write_word, bool  & dcache_r, bool mem_w0, bool mem_w1) 
{  
  auto addr = dcache_addr >> 1; 
  auto random = simulator().GetCycles() % 9;

  if (!random) 
  {
    dcache_r = false;
    read_word = 0xfeed ;
  }
  else 
  {
    dcache_r = true;
    read_word.range<15,8>() = GetUpperByteAt(addr).range<7,0>();
    read_word.range<7,0>() = GetLowerByteAt(addr).range<7,0>();
    if(mem_w0) 
      SetLowerByteAt(addr,write_word.range<7,0>());
    if(mem_w1)
      SetUpperByteAt(addr,write_word.range<15,8>());
  }
}
/***************************************************************/
/*                                                             */
/* icache_access                                               */
/*                                                             */
/***************************************************************/
void MainMemory::icache_access(bits16 icache_addr, bits16 & read_word, bool & icache_r) 
{	
  auto addr = icache_addr >> 1; 
  auto random = simulator().GetCycles() % 13;

  if (!random) 
  {
    icache_r = false;
    read_word = 0xfeed;
  }
  else 
  {
    icache_r = true;
    read_word.range<15,8>() = GetUpperByteAt(addr).range<7,0>();
    read_word.range<7,0>() = GetLowerByteAt(addr).range<7,0>();
  }
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a region of memory to the output file.     */
/*                                                             */
/***************************************************************/
void MainMemory::mdump(FILE * dumpsim_file, bits16 start, bits16 stop)
{
  uint16_t address; /* this is a byte address */

  printf("\nMemory content [0x%04x..0x%04x] :\n", start.to_num(), stop.to_num());
  printf("-------------------------------------\n");
  for (address = (start >> 1).to_num(); address <= (stop >> 1).to_num(); address++)
  {
    printf("  0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, GetUpperByteAt(address).to_num(), GetLowerByteAt(address).to_num());
  }
  
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%04x..0x%04x] :\n", start.to_num(), stop.to_num());
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1).to_num(); address <= (stop >> 1).to_num(); address++)
  {
    fprintf(dumpsim_file, " 0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, GetUpperByteAt(address).to_num(), GetLowerByteAt(address).to_num());
  }

  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}
