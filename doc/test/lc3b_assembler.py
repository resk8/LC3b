#!/usr/bin/env python3
"""
LC-3b Assembler
Assembles LC-3b assembly language files (.asm) into object files (.obj)
"""

import sys
import re
from typing import List, Tuple, Dict, Optional

class AssemblerError(Exception):
    """Custom exception for assembler errors"""
    pass

class LC3bAssembler:
    def __init__(self):
        self.symbols = {}  # Label -> address mapping
        self.origin = 0x3000
        self.current_address = 0x3000
        self.instructions = []  # List of (address, instruction_word, line_num, original_line)
        
        # Instruction opcodes
        self.opcodes = {
            'ADD': 0x1, 'AND': 0x5, 'BR': 0x0, 'BRN': 0x0, 'BRZ': 0x0, 'BRP': 0x0,
            'BRNZ': 0x0, 'BRNP': 0x0, 'BRZP': 0x0, 'BRNZP': 0x0,
            'JMP': 0xC, 'JSR': 0x4, 'JSRR': 0x4, 'LDB': 0x2, 'LDW': 0x6,
            'LEA': 0xE, 'RTI': 0x8, 'LSHF': 0xD, 'RSHFL': 0xD, 'RSHFA': 0xD,
            'STB': 0x3, 'STW': 0x7, 'TRAP': 0xF, 'XOR': 0x9, 'HALT': 0xF
        }
        
    def parse_register(self, reg_str: str, line_num: int) -> int:
        """Parse register string (R0-R7) and return register number"""
        reg_str = reg_str.strip().upper()
        if not re.match(r'^R[0-7]$', reg_str):
            raise AssemblerError(f"Line {line_num}: Invalid register '{reg_str}'")
        return int(reg_str[1])
    
    def parse_immediate(self, imm_str: str, line_num: int, bits: int, allow_label: bool = False) -> int:
        """Parse immediate value (decimal or hex)"""
        imm_str = imm_str.strip()
        
        # Check if it's a label (for .FILL directive)
        if allow_label and imm_str.upper() in self.symbols:
            return self.symbols[imm_str.upper()]
        
        try:
            if imm_str.startswith('#'):
                value = int(imm_str[1:], 10)
            elif imm_str.startswith('X') or imm_str.startswith('x'):
                value = int(imm_str[1:], 16)
            elif imm_str.startswith('0X') or imm_str.startswith('0x'):
                value = int(imm_str[2:], 16)
            else:
                value = int(imm_str, 10)
            
            # Check bounds
            min_val = -(1 << (bits - 1))
            max_val = (1 << (bits - 1)) - 1
            if value < min_val or value > max_val:
                raise AssemblerError(f"Line {line_num}: Immediate value {value} out of range [{min_val}, {max_val}]")
            
            # Convert to unsigned representation
            if value < 0:
                value = (1 << bits) + value
            
            return value
        except ValueError:
            raise AssemblerError(f"Line {line_num}: Invalid immediate value '{imm_str}'")
    
    def parse_offset(self, offset_str: str, current_addr: int, line_num: int, bits: int) -> int:
        """Parse PC offset (label or immediate)"""
        offset_str = offset_str.strip()
        
        # Check if it's a label
        label_upper = offset_str.upper()
        if label_upper in self.symbols:
            target_addr = self.symbols[label_upper]
            # LC-3b: addresses are in bytes, but PC offsets are in words (2 bytes)
            byte_offset = target_addr - (current_addr + 2)  # PC+2 addressing
            offset = byte_offset // 2  # Convert byte offset to word offset
            
            # Check if offset fits in bits
            min_offset = -(1 << (bits - 1))
            max_offset = (1 << (bits - 1)) - 1
            if offset < min_offset or offset > max_offset:
                raise AssemblerError(f"Line {line_num}: Branch offset {offset} out of range")
            
            # Convert to unsigned
            if offset < 0:
                offset = (1 << bits) + offset
            return offset
        else:
            # Parse as immediate
            return self.parse_immediate(offset_str, line_num, bits)
    
    def assemble_add_and(self, opcode: int, parts: List[str], line_num: int) -> int:
        """Assemble ADD/AND instruction"""
        if len(parts) != 4:
            raise AssemblerError(f"Line {line_num}: ADD/AND requires 3 operands")
        
        dr = self.parse_register(parts[1], line_num)
        sr1 = self.parse_register(parts[2], line_num)
        
        instruction = (opcode << 12) | (dr << 9) | (sr1 << 6)
        
        # Check if third operand is register or immediate
        if parts[3].strip().upper().startswith('R'):
            sr2 = self.parse_register(parts[3], line_num)
            instruction |= sr2
        else:
            imm5 = self.parse_immediate(parts[3], line_num, 5)
            instruction |= (1 << 5) | (imm5 & 0x1F)
        
        return instruction
    
    def assemble_branch(self, mnemonic: str, parts: List[str], current_addr: int, line_num: int) -> int:
        """Assemble BR instruction"""
        if len(parts) < 2:
            raise AssemblerError(f"Line {line_num}: BR requires 1 operand")
        
        # Determine condition codes
        n = z = p = 0
        if 'N' in mnemonic.upper():
            n = 1
        if 'Z' in mnemonic.upper():
            z = 1
        if 'P' in mnemonic.upper():
            p = 1
        
        # If just BR, set all flags (unconditional)
        if mnemonic.upper() == 'BR':
            n = z = p = 1
        
        nzp = (n << 2) | (z << 1) | p
        offset_str = ' '.join(parts[1:])
        offset9 = self.parse_offset(offset_str, current_addr, line_num, 9)
        
        return (nzp << 9) | (offset9 & 0x1FF)
    
    def assemble_jsr(self, parts: List[str], current_addr: int, line_num: int) -> int:
        """Assemble JSR instruction"""
        if len(parts) < 2:
            raise AssemblerError(f"Line {line_num}: JSR requires 1 operand")
        
        offset_str = ' '.join(parts[1:])
        offset11 = self.parse_offset(offset_str, current_addr, line_num, 11)
        return (0x4 << 12) | (1 << 11) | (offset11 & 0x7FF)
    
    def assemble_jsrr(self, parts: List[str], line_num: int) -> int:
        """Assemble JSRR instruction"""
        if len(parts) != 2:
            raise AssemblerError(f"Line {line_num}: JSRR requires 1 operand")
        
        base_r = self.parse_register(parts[1], line_num)
        return (0x4 << 12) | (base_r << 6)
    
    def assemble_load_store(self, opcode: int, parts: List[str], line_num: int) -> int:
        """Assemble LDW/STW/LDB/STB instruction"""
        if len(parts) != 4:
            raise AssemblerError(f"Line {line_num}: Load/Store requires 3 operands")
        
        dr_sr = self.parse_register(parts[1], line_num)
        base_r = self.parse_register(parts[2], line_num)
        offset6 = self.parse_immediate(parts[3], line_num, 6)
        
        return (opcode << 12) | (dr_sr << 9) | (base_r << 6) | (offset6 & 0x3F)
    
    def assemble_lea(self, parts: List[str], current_addr: int, line_num: int) -> int:
        """Assemble LEA instruction"""
        if len(parts) < 3:
            raise AssemblerError(f"Line {line_num}: LEA requires 2 operands")
        
        dr = self.parse_register(parts[1], line_num)
        # Join remaining parts in case label has spaces (shouldn't but be safe)
        offset_str = ' '.join(parts[2:])
        offset9 = self.parse_offset(offset_str, current_addr, line_num, 9)
        
        return (0xE << 12) | (dr << 9) | (offset9 & 0x1FF)
    
    def assemble_shift(self, mnemonic: str, parts: List[str], line_num: int) -> int:
        """Assemble LSHF/RSHFL/RSHFA instruction"""
        if len(parts) != 4:
            raise AssemblerError(f"Line {line_num}: Shift requires 3 operands")
        
        dr = self.parse_register(parts[1], line_num)
        sr = self.parse_register(parts[2], line_num)
        amount = self.parse_immediate(parts[3], line_num, 4)
        
        if amount > 15:
            raise AssemblerError(f"Line {line_num}: Shift amount must be 0-15")
        
        # Determine shift type
        if mnemonic.upper() == 'LSHF':
            shift_type = 0
        elif mnemonic.upper() == 'RSHFL':
            shift_type = 1
        else:  # RSHFA
            shift_type = 3
        
        return (0xD << 12) | (dr << 9) | (sr << 6) | (shift_type << 4) | (amount & 0xF)
    
    def assemble_trap(self, parts: List[str], line_num: int) -> int:
        """Assemble TRAP instruction"""
        if len(parts) != 2:
            raise AssemblerError(f"Line {line_num}: TRAP requires 1 operand")
        
        trapvect = self.parse_immediate(parts[1], line_num, 8)
        return (0xF << 12) | (trapvect & 0xFF)
    
    def assemble_jmp(self, parts: List[str], line_num: int) -> int:
        """Assemble JMP instruction"""
        if len(parts) != 2:
            raise AssemblerError(f"Line {line_num}: JMP requires 1 operand")
        
        base_r = self.parse_register(parts[1], line_num)
        return (0xC << 12) | (base_r << 6)
    
    def assemble_xor(self, parts: List[str], line_num: int) -> int:
        """Assemble XOR instruction"""
        if len(parts) != 4:
            raise AssemblerError(f"Line {line_num}: XOR requires 3 operands")
        
        dr = self.parse_register(parts[1], line_num)
        sr1 = self.parse_register(parts[2], line_num)
        
        instruction = (0x9 << 12) | (dr << 9) | (sr1 << 6)
        
        if parts[3].strip().upper().startswith('R'):
            sr2 = self.parse_register(parts[3], line_num)
            instruction |= sr2
        else:
            imm5 = self.parse_immediate(parts[3], line_num, 5)
            instruction |= (1 << 5) | (imm5 & 0x1F)
        
        return instruction
    
    def first_pass(self, lines: List[str]):
        """First pass: collect labels and calculate addresses"""
        self.current_address = self.origin
        
        for line_num, line in enumerate(lines, 1):
            # Remove comments
            if ';' in line:
                line = line[:line.index(';')]
            
            line = line.strip()
            if not line:
                continue
            
            # Check for .ORIG directive
            if line.upper().startswith('.ORIG'):
                parts = line.split()
                if len(parts) != 2:
                    raise AssemblerError(f"Line {line_num}: .ORIG requires 1 operand")
                self.origin = self.parse_immediate(parts[1], line_num, 16)
                self.current_address = self.origin
                continue
            
            # Check for .END directive
            if line.upper().startswith('.END'):
                break
            
            # Check for label
            label = None
            if ':' in line:
                label_part, rest = line.split(':', 1)
                label = label_part.strip().upper()
                if label in self.symbols:
                    raise AssemblerError(f"Line {line_num}: Duplicate label '{label}'")
                self.symbols[label] = self.current_address
                line = rest.strip()
            elif not line.upper().startswith('.'):
                # Check if line starts with a label (no colon, followed by instruction/directive)
                parts_temp = line.split()
                if len(parts_temp) >= 2:
                    first_word = parts_temp[0].upper()
                    second_word = parts_temp[1].upper()
                    # If first word is not an instruction and second is instruction/directive
                    if (first_word not in self.opcodes and 
                        (second_word in self.opcodes or second_word.startswith('.') or second_word.startswith('BR'))):
                        label = first_word
                        if label in self.symbols:
                            raise AssemblerError(f"Line {line_num}: Duplicate label '{label}'")
                        self.symbols[label] = self.current_address
                        line = ' '.join(parts_temp[1:])
            
            if not line:
                continue
            
            # Check for .FILL or .BLKW
            if line.upper().startswith('.FILL'):
                self.current_address += 2
            elif line.upper().startswith('.BLKW'):
                parts = line.split()
                if len(parts) != 2:
                    raise AssemblerError(f"Line {line_num}: .BLKW requires 1 operand")
                count = self.parse_immediate(parts[1], line_num, 16)
                self.current_address += 2 * count
            elif line.upper().startswith('.STRINGZ'):
                # Extract string
                match = re.search(r'"([^"]*)"', line)
                if not match:
                    raise AssemblerError(f"Line {line_num}: .STRINGZ requires a quoted string")
                string = match.group(1)
                self.current_address += 2 * (len(string) + 1)  # +1 for null terminator
            else:
                # Regular instruction
                self.current_address += 2
    
    def second_pass(self, lines: List[str]):
        """Second pass: generate machine code"""
        self.current_address = self.origin
        
        for line_num, line in enumerate(lines, 1):
            original_line = line
            
            # Remove comments
            if ';' in line:
                line = line[:line.index(';')]
            
            line = line.strip()
            if not line:
                continue
            
            # Check for .ORIG directive
            if line.upper().startswith('.ORIG'):
                parts = line.split()
                self.origin = self.parse_immediate(parts[1], line_num, 16)
                self.current_address = self.origin
                continue
            
            # Check for .END directive
            if line.upper().startswith('.END'):
                break
            
            # Remove label if present
            if ':' in line:
                _, line = line.split(':', 1)
                line = line.strip()
            elif not line.upper().startswith('.'):
                # Check if line starts with a label (no colon)
                parts_temp = line.split()
                if len(parts_temp) >= 2:
                    first_word = parts_temp[0].upper()
                    second_word = parts_temp[1].upper()
                    if (first_word not in self.opcodes and 
                        (second_word in self.opcodes or second_word.startswith('.') or second_word.startswith('BR'))):
                        line = ' '.join(parts_temp[1:])
            
            if not line:
                continue
            
            # Handle directives
            if line.upper().startswith('.FILL'):
                parts = line.split(None, 1)
                if len(parts) != 2:
                    raise AssemblerError(f"Line {line_num}: .FILL requires 1 operand")
                value = self.parse_immediate(parts[1], line_num, 16, allow_label=True)
                self.instructions.append((self.current_address, value & 0xFFFF, line_num, original_line))
                self.current_address += 2
                continue
            
            if line.upper().startswith('.BLKW'):
                parts = line.split()
                count = self.parse_immediate(parts[1], line_num, 16)
                for _ in range(count):
                    self.instructions.append((self.current_address, 0, line_num, original_line))
                    self.current_address += 2
                continue
            
            if line.upper().startswith('.STRINGZ'):
                match = re.search(r'"([^"]*)"', line)
                if not match:
                    raise AssemblerError(f"Line {line_num}: .STRINGZ requires a quoted string")
                string = match.group(1)
                for char in string:
                    self.instructions.append((self.current_address, ord(char), line_num, original_line))
                    self.current_address += 2
                self.instructions.append((self.current_address, 0, line_num, original_line))  # Null terminator
                self.current_address += 2
                continue
            
            # Parse instruction
            parts = re.split(r'[,\s]+', line)
            mnemonic = parts[0].upper()
            
            try:
                if mnemonic == 'ADD':
                    instruction = self.assemble_add_and(0x1, parts, line_num)
                elif mnemonic == 'AND':
                    instruction = self.assemble_add_and(0x5, parts, line_num)
                elif mnemonic.startswith('BR'):
                    instruction = self.assemble_branch(mnemonic, parts, self.current_address, line_num)
                elif mnemonic == 'JSR':
                    instruction = self.assemble_jsr(parts, self.current_address, line_num)
                elif mnemonic == 'JSRR':
                    instruction = self.assemble_jsrr(parts, line_num)
                elif mnemonic in ['LDW', 'STW', 'LDB', 'STB']:
                    opcode = {'LDB': 0x2, 'LDW': 0x6, 'STB': 0x3, 'STW': 0x7}[mnemonic]
                    instruction = self.assemble_load_store(opcode, parts, line_num)
                elif mnemonic == 'LEA':
                    instruction = self.assemble_lea(parts, self.current_address, line_num)
                elif mnemonic in ['LSHF', 'RSHFL', 'RSHFA']:
                    instruction = self.assemble_shift(mnemonic, parts, line_num)
                elif mnemonic == 'TRAP':
                    instruction = self.assemble_trap(parts, line_num)
                elif mnemonic == 'HALT':
                    instruction = 0xF025  # TRAP x25
                elif mnemonic == 'JMP':
                    instruction = self.assemble_jmp(parts, line_num)
                elif mnemonic == 'RET':
                    instruction = 0xC1C0  # JMP R7
                elif mnemonic == 'RTI':
                    instruction = 0x8000
                elif mnemonic == 'XOR':
                    instruction = self.assemble_xor(parts, line_num)
                else:
                    raise AssemblerError(f"Line {line_num}: Unknown instruction '{mnemonic}'")
                
                self.instructions.append((self.current_address, instruction, line_num, original_line))
                self.current_address += 2
                
            except AssemblerError:
                raise
            except Exception as e:
                raise AssemblerError(f"Line {line_num}: Error assembling '{line}': {str(e)}")
    
    def assemble_file(self, input_file: str, output_file: str):
        """Assemble an LC-3b assembly file"""
        try:
            with open(input_file, 'r') as f:
                lines = f.readlines()
            
            print(f"Assembling {input_file}...")
            
            # First pass: collect labels
            self.first_pass(lines)
            
            # Second pass: generate code
            self.second_pass(lines)
            
            # Write object file
            with open(output_file, 'w') as f:
                f.write(f"0x{self.origin:04X}\n")
                for addr, instruction, _, _ in self.instructions:
                    f.write(f"0x{instruction:04X}\n")
            
            print(f"Successfully assembled {len(self.instructions)} instructions")
            print(f"Output written to {output_file}")
            print(f"Origin: 0x{self.origin:04X}")
            print(f"Labels found: {len(self.symbols)}")
            if self.symbols:
                print("Symbol table:")
                for label, addr in sorted(self.symbols.items()):
                    print(f"  {label:20s} = 0x{addr:04X}")
            
        except FileNotFoundError:
            print(f"Error: File '{input_file}' not found")
            sys.exit(1)
        except AssemblerError as e:
            print(f"Assembly Error: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"Unexpected error: {e}")
            sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("LC-3b Assembler")
        print("Usage: python lc3b_assembler.py <input.asm> [output.obj]")
        print("\nExample:")
        print("  python lc3b_assembler.py program.asm")
        print("  python lc3b_assembler.py program.asm output.obj")
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    if len(sys.argv) >= 3:
        output_file = sys.argv[2]
    else:
        # Generate output filename from input
        if input_file.endswith('.asm'):
            output_file = input_file[:-4] + '.obj'
        else:
            output_file = input_file + '.obj'
    
    assembler = LC3bAssembler()
    assembler.assemble_file(input_file, output_file)
