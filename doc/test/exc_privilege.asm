; exc_privilege.asm — test PRIVILEGE_MODE_VIOLATION exception
; RTI executed while PSR[15]=1 (user mode, the simulator default) triggers
; a privilege mode violation. Expected: exception fires in MEM stage;
; instructions after RTI are squashed; pipeline jumps to handler at
; M[0x0200] (which is 0x0000 in these tests, halting the sim).

        .ORIG x3000

        AND R0, R0, #0          ; R0 = 0 (warm-up)
        ADD R0, R0, #1          ; R0 = 1
        LEA R6, FAKE_STACK      ; R6 = stack pointer (points into program memory)

        RTI                     ; PSR[15]=1 (user mode) -> PRIVILEGE_MODE_VIOLATION

        ADD R1, R1, #1          ; squashed — must NOT execute
        HALT                    ; squashed — must NOT execute

FAKE_STACK:
        .FILL x0000             ; [R6+0] placeholder
        .FILL x0000             ; [R6+2] placeholder

        .END
