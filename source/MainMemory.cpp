/***************************************************************/
/* Memory Implementaion                                        */
/***************************************************************/

#include <iostream>
#ifdef __linux__
    #include "../include/LC3b.h"
    #include "../include/Simulator.h"
    #include "../include/MainMemory.h"
#else    
    #include "LC3b.h"
    #include "Simulator.h"
    #include "MainMemory.h"
#endif

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void MainMemory::init_memory()
{
  int i;
  
  for (i=0; i < WORDS_IN_MEM; i++) 
  {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/*
*
*/
int MainMemory::GetLowerByteAt(int address) const
{
  try
  {
    return MEMORY.at(address).at(0);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Invalid Memory Location: addr=%x\n",address);
    printf("C++ error code : %s\n",oor.what());
	  Exit();
  }
}

/*
*
*/
void MainMemory::SetLowerByteAt(int address, int val)
{
  MEMORY[address][0] = val;
}

/*
*
*/
int MainMemory::GetUpperByteAt(int address) const
{
  try
  {
    return MEMORY.at(address).at(1);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Invalid Memory Location: addr=%x\n",address);
    printf("C++ error code : %s\n",oor.what());
	Exit();
  }
}

/*
*
*/
void MainMemory::SetUpperByteAt(int address, int val)
{
  MEMORY[address][1] = val;
}

/***************************************************************/
/*                                                             */
/* dcache_access                                               */
/*                                                             */
/***************************************************************/
void MainMemory::dcache_access(int dcache_addr, int *read_word, int write_word, int *dcache_r, int mem_w0, int mem_w1) 
{  
  int addr = dcache_addr >> 1 ; 
  int random = simulator().GetCycles() % 9;

  if (!random) 
  {
    *dcache_r = 0;
    *read_word = 0xfeed ;
  }
  else 
  {
    *dcache_r = 1;    
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
void MainMemory::icache_access(int icache_addr, int *read_word, int *icache_r) 
{	
  int addr = icache_addr >> 1 ; 
  int random = simulator().GetCycles() % 13;

  if (!random) 
  {
    *icache_r = 0;
    *read_word = 0xfeed;
  }
  else 
  {
    *icache_r = 1;
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
void MainMemory::mdump(FILE * dumpsim_file, int start, int stop)
{
  int address; /* this is a byte address */

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
