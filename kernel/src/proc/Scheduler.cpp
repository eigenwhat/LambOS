//
// Created by Martin Miralles-Cordal on 5/18/2026.
//

#include <proc/Scheduler.hpp>

#include <system/Debug.hpp>


/** Enqueues an embryo process into the dormant process list. */
Process * Scheduler::enqueueEmbryo(Process&& process)
{
   process.state = Process::State::Embryo;
   process.procState = nullptr;
   if (!dormantProcesses_.enqueue(std::move(process))) {
       return nullptr;
   }

   return &dormantProcesses_[dormantProcesses_.size() - 1];
}

/**
 * Marks an embryo process runnable once architecture-specific CPU state has
 * been attached.
 */
bool Scheduler::makeRunnable(Process& process, Process::ICpuState *cpuState)
{
   if (process.state != Process::State::Embryo ||
       cpuState == nullptr ||
       !process.addressSpace ||
       process.kernStack == nullptr)
   {
       return false;
   }

   // sys::debug_println("Making process %@ runnable", process.pid);
   process.procState = cpuState;
   process.state = Process::State::Runnable;
   return true;
}

Process * Scheduler::nextProcess()
{
   auto nextEligible = std::ranges::find_if(dormantProcesses_, [](Process const &p) {
       return p.state == Process::State::Runnable;
   });
   if (nextEligible != dormantProcesses_.end()) {
       return std::addressof(*nextEligible);
   }

   auto *curr = currentProcess();
   if (!curr) {
       return nullptr;
   }

   return (curr->state == Process::State::Running || curr->state == Process::State::Runnable)
           ? curr
           : nullptr;
}

/** Selects the next process and marks it as the current process. */
Process * Scheduler::promoteNextProcessToCurrent()
{
   if (auto next = nextProcess(); next != activeProcess_) {
       if (activeProcess_) {
           activeProcess_->state = Process::State::Runnable;
       }
       activeProcess_ = next;
   }

   return activeProcess_;
}
