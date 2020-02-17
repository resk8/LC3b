/***************************************************************/
/* MicroSequencer Implementaion                                */
/***************************************************************/

#include <iostream>
#ifdef __linux__
    #include "../include/LC3b.h"
    #include "../include/MicroSequencer.h"
#else    
    #include "LC3b.h"
    #include "MicroSequencer.h"
#endif

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void MicroSequencer::init_control_store(char *ucode_filename) 
{
  FILE *ucode;
  int i, j, index;
  char line[200];

  printf("Loading Control Store from file: %s\n", ucode_filename);

  /* Open the micro-code file. */
  if ((ucode = fopen(ucode_filename, "r")) == NULL) 
  {
    printf("Error: Can't open micro-code file %s\n", ucode_filename);
    Exit();
  }

  /* Read a line for each row in the control store. */
  for(i = 0; i < CONTROL_STORE_ROWS; i++) 
  {
    if (fscanf(ucode, "%[^\n]\n", line) == EOF) 
    {
      printf("Error: Too few lines (%d) in micro-code file: %s\n", i, ucode_filename);
      Exit();
    }  

    /* Put in bits one at a time. */
    index = 0;

    for (j = 0; j < NUM_CONTROL_STORE_BITS; j++) 
    {
      /* Needs to find enough bits in line. */
      if (line[index] == '\0') 
      {
        printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n", ucode_filename, i);
        Exit();
      }

      if (line[index] != '0' && line[index] != '1') 
      {
        printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n", ucode_filename, i, j);
        Exit();
      }

      /* Set the bit in the Control Store. */      
      SetMicroCodeBitsAt(i,j,(line[index] == '0') ? 0:1);
      index++;
    }

    /* Warn about extra bits in line. */
    if (line[index] != '\0')
    {
      printf("Warning: Extra bit(s) in control store file %s. Line: %d\n", ucode_filename, i);
    }
  }
  printf("\n");
}

/*
*
*/
int MicroSequencer::GetMicroCodeBitsAt(int index, int bits) const
{
  try
  {
    return CONTROL_STORE.at(index).at(bits);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: Invalid micro-code location: Index=%d, Bit=%d\n",index,bits);
    printf("C++ error code : ' %s '\n",oor.what());
	  Exit();
  }
}

/*
*
*/
void MicroSequencer::SetMicroCodeBitsAt(int index, int bit, int val)
{
  CONTROL_STORE[index][bit] = val;
}

/*
*
*/
void MicroSequencer::print_CS(int *CS, int num) const
{
  int ii ;
  for ( ii = 0 ; ii < num; ii++) {
    printf("%d",CS[ii]);
  }
  printf("\n");
}