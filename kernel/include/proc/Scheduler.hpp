//
// Created by Martin Miralles-Cordal on 12/23/2019.
//

#pragma once

#include <proc/Process.hpp>
#include <util/ArrayList.hpp>

#include <ranges>

class Scheduler
{
  public:
    Process * currentProcess() const { return activeProcess_; }
    Process * currentProcess() { return activeProcess_; }

    Process * nextProcess()
    {
        auto nextEligible = std::ranges::find_if(dormantProcesses_, [](Process const &p) {
            return p.state == Process::State::Runnable;
        });
        return nextEligible == dormantProcesses_.end() ? std::addressof(*nextEligible) : currentProcess();
    }

  private:
    Process *activeProcess_;
    sys::ArrayList<Process> dormantProcesses_;
};