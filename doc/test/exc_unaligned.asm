; exc_unaligned.asm — test ACV_OR_UNALIGNED exception (unaligned word access)
; LDW from an odd address triggers the unaligned access exception in MEM stage.
; Expected: exception fires; instructions after the LDW are squashed; pipeline
; jumps to handler at M[0x0204] (which is 0x0000 in these tests, halting sim).

        .ORIG x3000

        AND R0, R0, #0          ; R0 = 0 (warm-up)
        LEA R0, DATA_WORD       ; R0 = even address of DATA_WORD
        ADD R0, R0, #1          ; R0 is now odd -> unaligned word address

        LDW R1, R0, #0          ; word load from odd address -> ACV_OR_UNALIGNED

        ADD R2, R2, #1          ; squashed — must NOT execute
        HALT                    ; squashed — must NOT execute

DATA_WORD:
        .FILL x1234

        .END
