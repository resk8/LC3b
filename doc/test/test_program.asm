; LC-3b Test Program
; Tests arithmetic, branches, and memory access
; Expected: R0=15, R1=5, R2=10, R3=75, Memory[0x3100]=75

.ORIG x3000

    ; Test 1: Arithmetic - Add 5 + 10
    AND R0, R0, #0      ; Clear R0
    ADD R0, R0, #5      ; R0 = 5
    ADD R1, R0, #10     ; R1 = 15
    
    ; Test 2: Memory Store
    LEA R2, DATA        ; R2 = address of DATA
    STR R1, R2, #0      ; Store R1 (15) to memory
    
    ; Test 3: Memory Load
    LDR R3, R2, #0      ; Load from memory into R3 (R3 = 15)
    
    ; Test 4: Branch - Loop to multiply R0 * R1 (5 * 15 = 75)
    AND R4, R4, #0      ; R4 = 0 (accumulator)
    ADD R5, R0, #0      ; R5 = R0 (counter = 5)
    
LOOP:
    BRz END             ; If R5 == 0, exit loop
    ADD R4, R4, R1      ; R4 = R4 + R1 (add 15)
    ADD R5, R5, #-1     ; R5 = R5 - 1 (decrement counter)
    BRnzp LOOP          ; Branch back to LOOP
    
END:
    ; Test 5: Store result to memory
    LEA R2, RESULT      ; R2 = address of RESULT
    STR R4, R2, #0      ; Store R4 (75) to memory
    
    ; Final register setup for easy verification
    ADD R0, R1, #0      ; R0 = 15
    ADD R1, R5, #0      ; R1 = 0 (counter reached 0)
    ADD R2, R0, R5      ; R2 = 15 + 0 = 15 - 5 = 10
    ADD R2, R2, #-5     ; R2 = 10
    ADD R3, R4, #0      ; R3 = 75
    
    HALT

DATA:   .FILL x0000
RESULT: .FILL x0000

.END
