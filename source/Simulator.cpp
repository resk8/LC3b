/***************************************************************/
/* Simulator Implementaion                                     */
/***************************************************************/

#include <iostream>
#ifdef __linux__
    #include "../include/LC3b.h"
    #include "../include/PipeLine.h"
    #include "../include/MainMemory.h"
    #include "../include/State.h"
    #include "../include/MicroSequencer.h"
    #include "../include/Simulator.h"
#else    
    #include "LC3b.h"
    #include "PipeLine.h"
    #include "MainMemory.h"
    #include "State.h"
    #include "MicroSequencer.h"
    #include "Simulator.h"
#endif

/*
* Simulator constructor
*/
Simulator::Simulator() : 
CYCLE_COUNT(0), 
RUN_BIT(0)
{
  CpuPipeline = std::make_shared<PipeLine>(*this); 
  CpuMemory = std::make_shared<MainMemory>(*this);
  CpuState = std::make_shared<State>(*this);
  CpuMicroSequencer = std::make_shared<MicroSequencer>(*this);
}

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void Simulator::help() 
{
    printf("----------------LC-3bSIM Help-----------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the architectural state    \n");
    printf("idump            -  dump the internal state         \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void Simulator::cycle() 
{  
  pipeline().PropagatePipeLine();
  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void Simulator::run(int num_cycles) 
{
  if (RUN_BIT == FALSE) 
  {
    printf("Can't simulate, Simulator is halted\n\n");
  return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (auto i = 0; i < num_cycles; i++) 
  {
    if (state().GetProgramCounter() == 0x0000) 
    {
      cycle();
      RUN_BIT = FALSE;
      printf("Simulator halted\n\n");
      break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void Simulator::go() 
{
  if ((RUN_BIT == FALSE) || (state().GetProgramCounter() == 0x0000)) 
  {
	  printf("Can't simulate, Simulator is halted\n\n");
	  return;
  }
  
  printf("Simulating...\n\n");
  /* initialization */
  while (state().GetProgramCounter() != 0x0000)
  {
    cycle();
  }

  cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void Simulator::get_command(FILE * dumpsim_file) 
{
  char buffer[20];
  uint_16 start, stop, cycles;

  printf("LC-3b-SIM> ");
  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) 
  {
    case 'G':
    case 'g':
      go();
      break;
    case 'M':
    case 'm':
      scanf("%i %i", &start, &stop);
      memory().mdump(dumpsim_file, start, stop);
      break;
    case '?':
      help();
      break;
    case 'Q':
    case 'q':
      printf("Bye.\n");
      exit(0);
    case 'R':
    case 'r':
      if (buffer[1] == 'd' || buffer[1] == 'D')
      {
        state().rdump(dumpsim_file);
      }
      else 
      {
        scanf("%d", &cycles);
        run(cycles);
      }
      break;
    case 'I':
    case 'i':
      pipeline().idump(dumpsim_file);
      break;
    default:
      printf("Invalid Command\n");
      break;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void Simulator::load_program(char *program_filename) 
{
  uint_16 word, program_base;

  /* Open program file. */
  auto prog = fopen(program_filename, "r");
  if (prog == NULL) 
  {
    printf("Error: Can't open program file %s\n", program_filename);
    Exit();
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
  {
    program_base = word >> 1 ;
  }
  else 
  {
    printf("Error: Program file is empty\n");
    Exit();
  }

  auto ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) 
  {
    /* Make sure it fits. */
    auto program_memory = program_base + ii;
    if (program_memory >= WORDS_IN_MEM) 
    {
      printf("Error: Program file %s is too long to fit in memory. %x\n", program_filename, ii);
      Exit();
    }

    /* Write the word to memory array. */
    memory().SetLowerByteAt(program_base + ii, word & 0x00FF);
    memory().SetUpperByteAt(program_base + ii, (word >> 8) & 0x00FF);
    ii++;
  }

  if (state().GetProgramCounter() == 0) 
  {
    state().SetProgramCounter(program_base << 1);
  }
  
  printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void Simulator::initialize(char *ucode_filename, char *program_filename, uint_16 num_prog_files) 
{
  microsequencer().init_control_store(ucode_filename);
  memory().init_memory();
  state().init_state(); 
  pipeline().init_pipeline();

  for (auto i = 0; i < num_prog_files; i++ ) 
  {
	  load_program(program_filename);
	  while(*program_filename++ != '\0');
  }
     
  RUN_BIT = TRUE;
}

