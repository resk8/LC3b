; Example LC-3b Assembly Program
; Tests: arithmetic operations, branches, and memory access

        .ORIG x3000

        ; Clear R0 and load value 5
        AND R0, R0, #0
        ADD R0, R0, #5      ; R0 = 5
        
        ; Calculate R1 = R0 + 10 = 15
        ADD R1, R0, #10     ; R1 = 15
        
        ; Store R1 to memory
        LEA R2, DATA        ; R2 = address of DATA
        STW R1, R2, #0      ; Memory[DATA] = R1
        
        ; Load from memory into R3
        LDW R3, R2, #0      ; R3 = Memory[DATA]
        
        ; Multiply R0 * R1 using loop (5 * 15 = 75)
        AND R4, R4, #0      ; R4 = 0 (accumulator)
        ADD R5, R0, R0      ; R5 = R0 (counter = 5)
        
LOOP    BRz END             ; If R5 == 0, exit loop
        ADD R4, R4, R1      ; R4 = R4 + R1 (add 15)
        ADD R5, R5, #-1     ; R5 = R5 - 1
        BRnzp LOOP          ; Branch back to LOOP
        
END     LEA R2, RESULT      ; R2 = address of RESULT
        STW R4, R2, #0      ; Memory[RESULT] = R4
        
        ; Some more operations
        ADD R0, R1, R0      ; R0 = R1 + R0 = 20
        ADD R1, R5, #0      ; R1 = R5 = 0
        ADD R2, R0, R5      ; R2 = 20 + 0 = 20
        ADD R2, R2, #-5     ; R2 = 20 - 5 = 15
        ADD R3, R4, #0      ; R3 = R4 = 75
        
        HALT                ; Stop execution

DATA    .FILL x0000         ; Storage location
RESULT  .FILL x0000         ; Storage location

        .END
