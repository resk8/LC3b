; This program counts from 10 to 0
        .ORIG x3000

        LEA R0, TEN         ; Load address of TEN into R0
        LDW R1, R0, #0      ; Load value from TEN into R1
        
START   ADD R1, R1, #-1     ; Decrement R1
        BRz DONE            ; If zero, exit loop
        BRnzp START         ; Unconditional branch back to START
        
DONE    TRAP x25            ; HALT

TEN     .FILL x000A         ; Storage location with value 10

        .END
