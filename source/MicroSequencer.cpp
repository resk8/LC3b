/***************************************************************/
/* MicroSequencer Implementaion                                */
/***************************************************************/

#include <iostream>
#ifdef __linux__
    #include "../include/MicroSequencer.h"
#else    
    #include "MicroSequencer.h"
#endif

/*
*
*/
MicroSequencer::MicroSequencer(Simulator & intance) : _simulator(intance) 
{
  CONTROL_STORE = std::vector<std::vector<uint16_t>>(CONTROL_STORE_ROWS, std::vector<uint16_t>(NUM_CONTROL_STORE_BITS));
}

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
  char line[200];

  printf("Loading Control Store from file: %s\n", ucode_filename);

  /* Open the micro-code file. */
  if ((ucode = fopen(ucode_filename, "r")) == NULL) 
  {
    printf("Error: Can't open micro-code file %s\n", ucode_filename);
    Exit();
  }

  /* Read a line for each row in the control store. */
  for(auto i = 0; i < CONTROL_STORE_ROWS; i++) 
  {
    if (fscanf(ucode, "%[^\n]\n", line) == EOF) 
    {
      printf("Error: Too few lines (%d) in micro-code file: %s\n", i, ucode_filename);
      Exit();
    }  

    /* Put in bits one at a time. */
    auto index = 0;

    for (auto j = 0; j < NUM_CONTROL_STORE_BITS; j++) 
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
std::vector<uint16_t> & MicroSequencer::GetMicroCodeAt(uint16_t row)
{
  try
  {
    return CONTROL_STORE.at(row);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: trying to get Invalid micro-code location: Index=%d\n",row);
    printf("C++ error code : ' %s '\n",oor.what());
	  Exit();
  }
}

/*
*
*/
uint16_t MicroSequencer::GetMicroCodeBitsAt(uint16_t index, uint16_t bits) const
{
  try
  {
    return CONTROL_STORE.at(index).at(bits);
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: trying to get Invalid micro-code: Index=%d, Bit=%d\n",index,bits);
    printf("C++ error code : ' %s '\n",oor.what());
	  Exit();
  }
}

/*
*
*/
void MicroSequencer::SetMicroCodeBitsAt(uint16_t index, uint16_t bit, uint16_t val)
{
  try
  {
    CONTROL_STORE.at(index).at(bit) = val;
  }
  catch (const std::out_of_range& oor)
  {
    printf("\n********* C++ exception *********\n");
    printf("Error: trying to set Invalid micro-code: row=%d, Bit=%d\n",index, bit);
    printf("C++ error code : ' %s '\n",oor.what());
	  Exit();
  }
}

/*
*
*/
void MicroSequencer::print_CS(std::vector<uint16_t> &CS, int num) const
{
  for ( auto ii = 0 ; ii < num; ii++) {
    printf("%d",CS[ii]);
  }
  printf("\n");
}