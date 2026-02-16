; JMP / JSR / JSRR Test Program
; Expected final register state:
;   R0 = 1   (set inside SUBROUTINE_A, reached via JSR)
;   R1 = 2   (set inside SUBROUTINE_B, reached via JSRR)
;   R2 = 3   (set after JMP skips the trap, confirming JMP landed correctly)
;   R3 = 0   (never written — confirms skipped code was NOT executed)
;   R7 = address of AFTER_JSR   (saved by JSR, then overwritten by JSRR saved return)
;        actually R7 after JSRR = address of AFTER_JSRR
; Note: after RET (JMP R7) inside each subroutine, R7 still holds the return address

        .ORIG x3000

        ; ---- Test 1: JSR ----
        ; JSR saves PC+2 into R7 and jumps to SUBROUTINE_A
        JSR SUBROUTINE_A            ; R7 = addr of AFTER_JSR, PC -> SUBROUTINE_A

AFTER_JSR:
        ; ---- Test 2: JSRR ----
        ; Load address of SUBROUTINE_B into R4, then JSRR R4
        LEA R4, SUBROUTINE_B        ; R4 = address of SUBROUTINE_B
        JSRR R4                     ; R7 = addr of AFTER_JSRR, PC -> SUBROUTINE_B

AFTER_JSRR:
        ; ---- Test 3: JMP ----
        ; JMP over the trap to LAND, confirming unconditional jump
        LEA R5, LAND                ; R5 = address of LAND
        JMP R5                      ; PC -> LAND (skips the ADD below)

        ADD R3, R3, #7              ; R3 = 7 (should NOT execute)

LAND:
        ADD R2, R2, #3              ; R2 = 3 (confirm we landed here)

        HALT

; ---- Subroutine A (reached by JSR) ----
SUBROUTINE_A:
        ADD R0, R0, #1              ; R0 = 1
        JMP R7                      ; return to AFTER_JSR

; ---- Subroutine B (reached by JSRR) ----
SUBROUTINE_B:
        ADD R1, R1, #2              ; R1 = 2
        JMP R7                      ; return to AFTER_JSRR

        .END
