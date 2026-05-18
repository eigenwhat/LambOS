//
// Created by Martin Miralles-Cordal on 12/23/2019.
//

#pragma once

#include <proc/Process.hpp>
#include <util/ArrayList.hpp>

#include <ranges>
#include <utility>

class Scheduler
{
  public:
    Process const * currentProcess() const { return activeProcess_; }
    Process       * currentProcess()       { return activeProcess_; }
    void setCurrentProcess(Process *process) { activeProcess_ = process; }

    bool hasRunnableProcess() const
    {
        return std::ranges::find_if(dormantProcesses_, [](Process const &p) {
            return p.state == Process::State::Runnable;
        }) != dormantProcesses_.end();
    }

    /** Enqueues an embryo process into the dormant process list. */
    Process *enqueueEmbryo(Process&& process);

    /**
     * Marks an embryo process runnable once architecture-specific CPU state has
     * been attached.
     */
    bool makeRunnable(Process& process, Process::ICpuState *cpuState);

    Process * nextProcess();

    /** Selects the next process and marks it as the current process. */
    Process *promoteNextProcessToCurrent();

    Process::ID nextPid() { return nextPid_++; }

  private:
    Process *activeProcess_ = nullptr;
    sys::ArrayList<Process> dormantProcesses_;
    Process::ID nextPid_ = 0;
};
