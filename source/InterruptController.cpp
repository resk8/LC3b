/***************************************************************/
/* instruction.cpp: LC-3b Instruction Class Source File        */
/***************************************************************/

#ifdef __linux__ 
    #include "../include/InterruptController.h"
    #include "../include/State.h"    
#else
    #include "InterruptController.h"
    #include "State.h"
#endif

/**
* @brief Construct a new InterruptController object.
*
* @param instance A reference to the Simulator instance, allowing
*                 the interrupt controller to access CPU state and
*                 signals as needed.
*/
InterruptController::InterruptController(Simulator & instance) : _simulator(instance) 
{
    setSavedUSP(0);
    setSSP(0x3000); // Initialize SSP to top of supervisor stack
}

/**
 * @brief Check if there is a pending interrupt that should be serviced.
 *
 * @return true if the highest-priority pending request has priority
 *         greater than the current CPU priority level in PSR[10:8].
 */
bool
InterruptController::HasPendingInterrupt()
{
    if (pending.empty()) return false;
    // Only interrupt if device priority > current CPU priority
    return pending.top().priority.to_num() > simulator().state().GetPriorityLevel().to_num();
}

/**
 * @brief Queue a new interrupt request from a device.
 *
 * @param priority The 3-bit priority level of the interrupting device.
 * @param vector   The 8-bit interrupt vector (e.g., 0x80 for keyboard).
 *                 The CPU will read the ISR address from M[x01vv].
 */
void
InterruptController::RaiseInterrupt(bits3 priority, bits8 vector)
{
    InterruptRequest req;
    req.vector = 0x0100 + (vector.to_num() << 1); // Calculate vector address (x02vv)
    req.priority = priority;
    pending.push(req);
}

/**
 * @brief Acknowledge and dequeue the highest-priority interrupt request.
 *
 * Called by the pipeline when it begins servicing the interrupt.
 * Pops the top request off the priority queue and returns it so
 * the pipeline can use the vector and priority for the service routine.
 *
 * @return The InterruptRequest with the highest priority.
 */
InterruptRequest
InterruptController::ProcessInterrupt()
{
    auto req = pending.top();
    pending.pop();
    return req;
}