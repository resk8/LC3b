/***************************************************************/
/* MicroSequencer Implementaion                                */
/***************************************************************/

#include <cstring>
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
  int ucode_row = 0;
  while(ucode_row < CONTROL_STORE_ROWS)
  {
    if (fscanf(ucode, "%[^\n]\n", line) == EOF)
    {
      printf("Error: Too few lines (%d) in micro-code file: %s. Expected %d lines.\n", ucode_row, ucode_filename, CONTROL_STORE_ROWS);
      Exit();
    }

    /* Find the '#' character and terminate the string there to ignore comments */
    char* comment_start = strchr(line, '#');
    if (comment_start != NULL)
    {
        *comment_start = '\0';
    }

    // Check if the line is empty after stripping comments
    bool is_empty = true;
    for (int j = 0; line[j] != '\0'; j++) {
        if (!isspace(line[j])) {
            is_empty = false;
            break;
        }
    }

    if (is_empty) {
        continue; // Skip empty or comment-only lines
    }

    /* Put in bits one at a time. */
    int bit_count = 0;

    for (int j = 0; line[j] != '\0' && bit_count < NUM_CONTROL_STORE_BITS; j++)
    {
      if (line[j] == '0' || line[j] == '1')
      {
        /* Set the bit in the Control Store. */
        SetMicroCodeBitsAt(ucode_row, bit_count, (line[j] == '0') ? 0 : 1);
        bit_count++;
      }
      else if (!isspace(line[j]))
      {
        printf("Error: Unknown value '%c' in micro-code file: %s\nLine: %d\n", line[j], ucode_filename, ucode_row);
        Exit();
      }
    }

    if (bit_count < NUM_CONTROL_STORE_BITS)
    {
      printf("Error: Too few control bits in micro-code file: %s\nLine: %d (found %d, expected %d)\n", ucode_filename, ucode_row, bit_count, NUM_CONTROL_STORE_BITS);
      Exit();
    }

    /* Warn about extra bits in line. */
    for (int j = 0; line[j] != '\0'; j++)
    {
        if (!isspace(line[j]) && line[j] != '0' && line[j] != '1') {
             printf("Warning: Extra character(s) '%c' in control store file %s. Line: %d\n", line[j], ucode_filename, ucode_row);
        }
    }

    ucode_row++; // Increment only when a valid line is processed
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
