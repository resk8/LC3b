/***************************************************************/
/* Disassembler.h: LC-3b Disassembler Class Header File        */
/***************************************************************/
#pragma once

#include <string>
#ifdef __linux__ 
    #include "../include/LC3b.h"
#else
    #include "LC3b.h"
#endif

class Disassembler {
public:
    static std::string disassemble(bits16 instruction);

private:
    static std::string format_branch(bits16 instruction);
    static std::string format_operate(bits16 instruction);
    static std::string format_shift(bits16 instruction);
};