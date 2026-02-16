; RTI Test Program
; Tests RTI instruction (Return from Interrupt)
; Requires supervisor mode (PSR[15]=0)
; Stack grows downward from 0x3000.
; After an interrupt pushes PC then PSR, R6 = 0x3000 - 4 = 0x2ffc:
;   Memory[0x2ffc] = saved PC   (R6 + 0)
;   Memory[0x2ffe] = saved PSR  (R6 + 2)
; RTI pops both words and restores R6 to 0x3000.
; (This test places the stack in program memory for simplicity.)

        .ORIG x3000

        ; Setup R6 as stack pointer (pointing to 0x3020)
        LEA R6, STACK           ; R6 = address of STACK (0x3020)

        ; Load address of RETURN_POINT into R7
        LEA R7, RETURN_POINT    ; R7 = address of RETURN_POINT

        ; Write saved PC to stack: Memory[R6+0] = RETURN_POINT
        STW R7, R6, #0

        ; Set up a PSR value (0x0002 = supervisor mode, Z=1)
        AND R0, R0, #0          ; R0 = 0
        ADD R0, R0, #2          ; R0 = 2
        STW R0, R6, #1          ; Memory[R6+2] = 0x0002 (saved PSR)

        ; Execute RTI - should:
        ; 1. Read PC from [R6]    -> jump to RETURN_POINT
        ; 2. Read PSR from [R6+2] -> restore privilege bit
        ; 3. R6 += 4
        RTI

        ; These instructions should NOT execute
        ADD R1, R1, #7          ; R1 = 7 (should NOT happen)
        ADD R1, R1, #7          ; R1 = 14 (should NOT happen)
        ADD R1, R1, #7          ; R1 = 21 (should NOT happen)

RETURN_POINT:
        ; These instructions SHOULD execute after RTI
        ADD R2, R2, #5          ; R2 = 5 (verify we landed here)
        ADD R3, R3, #1          ; R3 = 1

        HALT

STACK:  .FILL x0000             ; [R6+0] = saved PC
        .FILL x0000             ; [R6+2] = saved PSR

        .END
