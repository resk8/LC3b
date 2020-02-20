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
  MEMORY = std::vector<std::vector<uint16_t>>(WORDS_IN_MEM,std::vector<uint16_t>(2));
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
*
*/
uint16_t MainMemory::GetLowerByteAt(uint16_t address) const
{
  try
  {
    return MEMORY.at(address).at(0);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Low byte read to invalid memory location: addr=%x\n",address);
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
*
*/
void MainMemory::SetLowerByteAt(uint16_t address, uint16_t val)
{
  try
  {
    MEMORY.at(address).at(0) = val;
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Low byte write to invalid memory location: addr=%x\n",address);
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
*
*/
uint16_t MainMemory::GetUpperByteAt(uint16_t address) const
{
  try
  {
    return MEMORY.at(address).at(1);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: High byte read to invalid memory location: addr=%x\n",address);
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
*
*/
void MainMemory::SetUpperByteAt(uint16_t address, uint16_t val)
{
  try
  {
    MEMORY.at(address).at(1) = val;
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: High byte write to invalid memory location: addr=%x\n",address);
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/***************************************************************/
/*                                                             */
/* dcache_access                                               */
/*                                                             */
/***************************************************************/
void MainMemory::dcache_access(uint16_t dcache_addr, uint16_t *read_word, uint16_t write_word, uint16_t *dcache_r, uint16_t mem_w0, uint16_t mem_w1) 
{  
  auto addr = dcache_addr >> 1 ; 
  auto random = simulator().GetCycles() % 9;

  if (!random) 
  {
    *dcache_r = false;
    *read_word = 0xfeed ;
  }
  else 
  {
    *dcache_r = true;    
    *read_word = (GetUpperByteAt(addr) << 8) | (GetLowerByteAt(addr) & 0x00FF);
    if(mem_w0) 
      SetLowerByteAt(addr,write_word & 0x00FF);
    if(mem_w1)
      SetUpperByteAt(addr,(write_word & 0xFF00) >> 8);
  }
}
/***************************************************************/
/*                                                             */
/* icache_access                                               */
/*                                                             */
/***************************************************************/
void MainMemory::icache_access(uint16_t icache_addr, uint16_t *read_word, uint16_t *icache_r) 
{	
  auto addr = icache_addr >> 1 ; 
  auto random = simulator().GetCycles() % 13;

  if (!random) 
  {
    *icache_r = false;
    *read_word = 0xfeed;
  }
  else 
  {
    *icache_r = true;
    *read_word = GetUpperByteAt(addr) << 8 | GetLowerByteAt(addr);
  }
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a region of memory to the output file.     */
/*                                                             */
/***************************************************************/
void MainMemory::mdump(FILE * dumpsim_file, uint16_t start, uint16_t stop)
{
  uint16_t address; /* this is a byte address */

  printf("\nMemory content [0x%04x..0x%04x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
  {
    printf("  0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, GetUpperByteAt(address), GetLowerByteAt(address));
  }
  
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%04x..0x%04x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
  {
    fprintf(dumpsim_file, " 0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, GetUpperByteAt(address), GetLowerByteAt(address));
  }

  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}
