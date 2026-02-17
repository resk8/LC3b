/***************************************************************/
/* InterruptController.h: LC-3b Interrupt Controller Header File */
/***************************************************************/
#pragma once

#include <vector>
#include <queue>
#ifdef __linux__ 
    #include "../include/LC3b.h"
    #include "../include/Simulator.h"
#else
    #include "LC3b.h"
    #include "Simulator.h"
#endif

// Interrupt state machine
enum INT_State {
    INT_IDLE,
    INT_SWAP_STACK,   // Save USP, load SSP into R6
    INT_PUSH_PC,      // Write PC to supervisor stack
    INT_PUSH_PSR,     // Write PSR to supervisor stack, update PSR
    INT_VECTOR_READ   // Read ISR address from vector table, set PC, release stall
};

struct InterruptRequest {
    bits3 priority; // 0-7, where 7 is the highest priority
    bits8 vector; // Address of the interrupt handler routine
};

class Simulator;
class InterruptController {
public:
    InterruptController(Simulator & instance);
    ~InterruptController() {}

    Simulator & simulator() { return _simulator; }

    bool HasPendingInterrupt();
    void RaiseInterrupt(bits3 priority, bits8 vector);
    InterruptRequest ProcessInterrupt();

    void setIntState(INT_State state) { int_state = state; }
    void setSavedUSP(bits16 usp) { saved_usp = usp; }
    void setSSP(bits16 ssp_val) { ssp = ssp_val; }
    void ClearPendingInterrupts() { while (!pending.empty()) pending.pop(); }
    void setIntPriority(bits3 priority) { int_priority = priority; }
    void setIntVector(bits8 vector) { int_vector = vector; }

    INT_State getIntState() { return int_state; }
    bits16 getSavedUSP() { return saved_usp; }
    bits16 getSSP() { return ssp; }
    bits3 getIntPriority() { return int_priority; }
    bits8 getIntVector() { return int_vector; }

private:
    Simulator & _simulator;

    INT_State int_state;
    bits16 saved_usp;
    bits16 ssp;

    bits3 int_priority;
    bits8 int_vector;

    struct ComparePriority {
        bool operator()(const InterruptRequest& a, const InterruptRequest& b) {
            return a.priority.to_num() < b.priority.to_num(); // Higher priority first
        }
    };
    std::priority_queue<InterruptRequest, std::vector<InterruptRequest>, ComparePriority> pending;
};