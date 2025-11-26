/***************************************************************/
/* Disassembler Implementation                                  */
/***************************************************************/

#include <sstream>
#include <map>
#ifdef __linux__
    #include "../include/Disassembler.h"
#else
    #include "Disassembler.h"
#endif


std::string Disassembler::disassemble(bits16 instruction) {
    std::stringstream ss;
    auto opcode = instruction.range<15, 12>();

    // Using a map for trap vectors to make it cleaner
    static const std::map<int, std::string> trap_map = {
        {0x20, "GETC"},
        {0x21, "OUT"},
        {0x22, "PUTS"},
        {0x23, "IN"},
        {0x24, "PUTSP"},
        {0x25, "HALT"}
    };

    switch (opcode.to_num()) {
        case 0b0000: // BR
            return format_branch(instruction);
        case 0b0001: // ADD
        case 0b0101: // AND
        case 0b1001: // XOR/NOT
            return format_operate(instruction);
        case 0b0010: // LD
        {
            auto dr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto pc_offset_9 = static_cast<int16_t>(instruction.range<8, 0>().sign_ext().to_num());
            ss << "LD R" << dr << ", #" << pc_offset_9;
            return ss.str();
        }
        case 0b0011: // ST
        {
            auto sr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto pc_offset_9 = static_cast<int16_t>(instruction.range<8, 0>().sign_ext().to_num());
            ss << "ST R" << sr << ", #" << pc_offset_9;
            return ss.str();
        }
        case 0b0100: // JSR/JSRR
        {
            if (instruction[11]) { // JSR
                auto pc_offset_11 = static_cast<int16_t>(instruction.range<10, 0>().sign_ext().to_num());
                ss << "JSR #" << pc_offset_11;
            } else { // JSRR
                auto base_r = static_cast<int>(instruction.range<8, 6>().to_num());
                ss << "JSRR R" << base_r;
            }
            return ss.str();
        }
        case 0b0110: // LDR
        {
            auto dr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto base_r = static_cast<int>(instruction.range<8, 6>().to_num());
            auto offset6_bits = instruction.range<5, 0>();
            int16_t offset6 = offset6_bits.to_num();
            if (offset6_bits[5]) { // Check the 6th bit (the sign bit)
                offset6 |= 0xFFC0; // Manually sign-extend to 16 bits
            }
            ss << "LDR R" << dr << ", R" << base_r << ", #" << offset6;
            return ss.str();
        }
        case 0b0111: // STR
        {
            auto sr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto base_r = static_cast<int>(instruction.range<8, 6>().to_num());
            auto offset6_bits = instruction.range<5, 0>();
            int16_t offset6 = offset6_bits.to_num();
            if (offset6_bits[5]) { // Check the 6th bit (the sign bit)
                offset6 |= 0xFFC0; // Manually sign-extend to 16 bits
            }
            ss << "STR R" << sr << ", R" << base_r << ", #" << offset6;
            return ss.str();
        }
        case 0b1000: // RTI
            return "RTI";
        case 0b1010: // LDI
        {
            auto dr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto pc_offset_9 = static_cast<int16_t>(instruction.range<8, 0>().sign_ext().to_num());
            ss << "LDI R" << dr << ", #" << pc_offset_9;
            return ss.str();
        }
        case 0b1011: // STI
        {
            auto sr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto pc_offset_9 = static_cast<int16_t>(instruction.range<8, 0>().sign_ext().to_num());
            ss << "STI R" << sr << ", #" << pc_offset_9;
            return ss.str();
        }
        case 0b1100: // JMP
        {
            auto base_r = static_cast<int>(instruction.range<8, 6>().to_num());
            if (base_r == 7) { // RET is an alias for JMP R7
                return "RET";
            }
            ss << "JMP R" << base_r;
            return ss.str();
        }
        case 0b1101: // SHF
            return format_shift(instruction);
        case 0b1110: // LEA
        {
            auto dr = static_cast<int>(instruction.range<11, 9>().to_num());
            auto pc_offset_9 = static_cast<int16_t>(instruction.range<8, 0>().sign_ext().to_num());
            ss << "LEA R" << dr << ", #" << pc_offset_9;
            return ss.str();
        }
        case 0b1111: // TRAP
        {
            auto trapvect8 = static_cast<int>(instruction.range<7, 0>().to_num());
            auto it = trap_map.find(trapvect8);
            if (it != trap_map.end()) {
                return it->second;
            }
            ss << "TRAP x" << std::hex << std::uppercase << trapvect8;
            return ss.str();
        }

        default:
            return "UNKNOWN";
    }
}

std::string Disassembler::format_branch(bits16 instruction) {
    std::stringstream ss;
    bool n = instruction[11];
    bool z = instruction[10];
    bool p = instruction[9];
    auto pc_offset_9 = static_cast<int16_t>(instruction.range<8, 0>().sign_ext().to_num());

    ss << "BR";
    if (n) ss << "n";
    if (z) ss << "z";
    if (p) ss << "p";
    if (!n && !z && !p) { // This is how some assemblers handle NOP
        return "NOP";
    }
    ss << " #" << pc_offset_9;
    return ss.str();
}

std::string Disassembler::format_operate(bits16 instruction) {
    std::stringstream ss;
    auto opcode = instruction.range<15, 12>().to_num();
    auto dr = static_cast<int>(instruction.range<11, 9>().to_num());
    auto sr1 = static_cast<int>(instruction.range<8, 6>().to_num());

    if (opcode == 0b0001) ss << "ADD ";
    else if (opcode == 0b0101) ss << "AND ";
    else if (opcode == 0b1001) { // XOR or NOT
        if (instruction[5] == 1 && instruction.range<4,0>().to_num() == 0x1F) {
             ss << "NOT R" << dr << ", R" << sr1;
             return ss.str();
        }
        ss << "XOR ";
    }

    ss << "R" << dr << ", R" << sr1;

    if (instruction[5] == 0) { // Register mode
        auto sr2 = static_cast<int>(instruction.range<2, 0>().to_num());
        ss << ", R" << sr2;
    } else { // Immediate mode
        auto imm5 = static_cast<int16_t>(instruction.range<4, 0>().sign_ext().to_num());
        ss << ", #" << imm5;
    }
    return ss.str();
}

std::string Disassembler::format_shift(bits16 instruction) {
    std::stringstream ss;
    auto dr = static_cast<int>(instruction.range<11, 9>().to_num());
    auto sr = static_cast<int>(instruction.range<8, 6>().to_num());
    auto amount4 = static_cast<int>(instruction.range<3, 0>().to_num());
    auto shift_type = static_cast<int>(instruction.range<5, 4>().to_num());

    switch (shift_type) {
        case 0b00: // LSHF
            ss << "LSHF ";
            break;
        case 0b01: // RSHFL (logical)
            ss << "RSHFL ";
            break;
        case 0b11: // RSHFA (arithmetic)
            ss << "RSHFA ";
            break;
        default:
            return "UNKNOWN SHIFT";
    }

    ss << "R" << dr << ", R" << sr << ", #" << amount4;
    return ss.str();
}