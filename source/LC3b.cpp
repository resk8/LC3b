/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

#include <iostream>
#include "LC3b.h"
#include "Simulator.h"

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) 
{
  FILE * dumpsim_file;
  Simulator Simulator;

  /* Error Checking */
  if (argc < 3) 
  {
	  printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n", argv[0]);
	  exit(1);
  }

  printf("LC-3b Simulator\n\n");
  Simulator.initialize(argv[1], argv[2], argc - 2);

  if ( (dumpsim_file = fopen( "dumpsim.txt", "w" )) == NULL ) 
  {
	  printf("Error: Can't open dumpsim file\n");
	  Exit();
  }

  while (1)
  {
	  Simulator.get_command(dumpsim_file);
  }
}