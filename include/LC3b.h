#pragma once

#include <type_traits>
#ifdef __linux__ 
    #include "../include/BitField.h"
#else
    #include "BitField.h"
#endif


/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define TRUE  1
#define FALSE 0

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

#ifdef __linux__ 
    #include <cstdlib>
    inline void Exit() { exit(-1); }
#else
    __attribute__((noreturn))
    inline void Exit() { system("pause"); exit(-1); }
#endif