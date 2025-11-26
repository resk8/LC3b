/***************************************************************/
/* MicroSequencer Implementaion                                */
/***************************************************************/

#ifdef __linux__
    #include "../include/MicroSequencer.h"
#else
    #include "MicroSequencer.h"
#endif

/*
* //TODO
*/
MicroSequencer::MicroSequencer(Simulator & intance) : _simulator(intance)
{
  CONTROL_STORE = std::vector<cs_bits>(CONTROL_STORE_ROWS, cs_bits());
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
* //TODO
*/
cs_bits & MicroSequencer::GetMicroCodeAt(uint8_t row)
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
* //TODO
*/
bool MicroSequencer::GetMicroCodeBitsAt(uint8_t index, uint8_t bits) const
{
  try
  {
    return CONTROL_STORE.at(index)[bits];
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
* //TODO
*/
void MicroSequencer::SetMicroCodeBitsAt(uint8_t index, uint8_t bit, bool val)
{
  try
  {
    CONTROL_STORE.at(index)[bit] = val;
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
* //TODO
*/
void MicroSequencer::print_CS(const cs_bits &CS, int num) const
{
  printf("CS :");
  for ( auto ii = 0 ; ii < num; ii++) {
    printf("%d",CS[ii]);
  }
  printf("\n");
}

void MicroSequencer::cdump(FILE * dumpsim_file) const
{
  printf("\nControl store content \n");
  printf("------------------------\n");

  for (auto row = 0 ; row < CONTROL_STORE_ROWS; row++)
  {
    printf("Row %d :",row);
    for ( auto ii = 0 ; ii < NUM_CONTROL_STORE_BITS; ii++) {
      printf("%d",CONTROL_STORE[row][ii]);
    }
    printf("\n");
  }

  fprintf(dumpsim_file, "\nControl store content \n");
  fprintf(dumpsim_file, "------------------------\n");

  for (auto row = 0 ; row < CONTROL_STORE_ROWS; row++)
  {
    fprintf(dumpsim_file, "Row %d :",row);
    for ( auto ii = 0 ; ii < NUM_CONTROL_STORE_BITS; ii++) {
      fprintf(dumpsim_file, "%d",CONTROL_STORE[row][ii]);
    }
    fprintf(dumpsim_file, "\n");
  }

  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}
