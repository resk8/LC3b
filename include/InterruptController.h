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

    void setSavedUSP(bits16 usp) { saved_usp = usp; }
    void setSSP(bits16 ssp_val) { ssp = ssp_val; }
    void ClearPendingInterrupts() { while (!pending.empty()) pending.pop(); }

    bits16 getSavedUSP() { return saved_usp; }
    bits16 getSSP() { return ssp; }

private:
    Simulator & _simulator;

    bits16 saved_usp;
    bits16 ssp;

    struct ComparePriority {
        bool operator()(const InterruptRequest& a, const InterruptRequest& b) {
            return a.priority.to_num() < b.priority.to_num(); // Higher priority first
        }
    };
    std::priority_queue<InterruptRequest, std::vector<InterruptRequest>, ComparePriority> pending;
};