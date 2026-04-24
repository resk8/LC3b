; exc_illegal.asm — test ILLEGAL_INSTRUCTION exception
; Opcode 0xA (bits[15:12]=1010) is reserved. Encoded via .FILL xA000.
; Expected: exception fires in DE stage; the .FILL word and subsequent
; instructions are squashed; pipeline drains then jumps to handler at
; M[0x0202] (which is 0x0000 in these tests, halting the sim).

        .ORIG x3000

        AND R0, R0, #0          ; R0 = 0 (warm-up)
        ADD R0, R0, #3          ; R0 = 3

        .FILL #-24576           ; bit pattern 0xA000 — reserved opcode -> ILLEGAL_INSTRUCTION

        ADD R1, R1, #1          ; squashed — must NOT execute
        ADD R1, R1, #2          ; squashed — must NOT execute
        HALT                    ; squashed — must NOT execute

        .END
