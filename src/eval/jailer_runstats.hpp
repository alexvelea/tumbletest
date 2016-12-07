#pragma once

#include <sys/resource.h>   /// rusage
#include <stddef.h>         /// size_t
#include <string>

#include "../utils/string.hpp"

namespace tumbletest {

/// Statistics about the task
class RunStats {
  public:
    /// List of result codes we can expect
    /// Anything except OK is a fail
    enum ResultCode {
        UNDEFINED = 0,
        OK,                             /// Okay
        RESTRICTED_FUNCTION,            /// Restricted system call
        TIME_LIMIT_EXCEEDED,            /// Time limit exceeded
        WALL_TIME_LIMIT_EXCEEDED,       /// Wall time limit exceeded
        MEMORY_LIMIT_EXCEEDED,          /// Memory limit Exceed
        OUTPUT_LIMIT_EXCEEDED,          /// Output limit Exceed
        NON_ZERO_EXIT_STATUS,           /// Process did not return 0
        RUNTIME_ERROR,                  /// Run time error (SIGSEGV, ...)
        ABNORMAL_TERMINATION,           /// Abnormal Termination WAT?
        INTERNAL_ERROR,                 /// Internal Jail Error
    };

    struct TimeStat {
        unsigned long long wallTimeMs;      /// wall clock time usage in ms
        unsigned long long cpuTimeMs;       /// CPU usage time in ms
        unsigned long long userTimeMs;      /// CPU usage in user mode in ms
        unsigned long long systemTimeMs;    /// CPU usage in kernel (system) mode in ms
    };

    RunStats() {
        this->timeStat = {0, 0, 0, 0};

        this->memoryKB = 0;
        
        this->rssPeak = 0;
        this->cswVoluntary = 0;
        this->cswForced = 0;
        this->softPageFaults = 0;
        this->hardPageFaults = 0;

        this->nrSysCalls = 0;
        this->lastSysCall = 0;
        this->terminalSignal = 0;

        this->exitCode = 0;
        this->processWasKilled = false;
        this->resultCode = INTERNAL_ERROR;

        this->internalMessage = "";
    }

    TimeStat timeStat;

    size_t memoryKB;            /// memory as queried from control group

    long int rssPeak;           /// resident set peak size (bytes) -- amount of memory in RAM, not swap
    long int cswVoluntary;      /// number of voluntary context switches
    long int cswForced;         /// number of forced context switches
    size_t softPageFaults;      /// minor page faults (number of pages)
    size_t hardPageFaults;      /// major page faults (number of pages)

    int nrSysCalls;             /// nr of system calls we intercepted
    int lastSysCall;            /// last syscall code called
    int terminalSignal;         /// signal that killed the process

    int exitCode;               /// exit code (that the program terminated with naturally)
    bool processWasKilled;      /// true if it was killed by keeper (time limits)
    ResultCode resultCode;      /// if not ResultCode::OK, it's the reason the task did not pass

    static const std::string version;
    std::string internalMessage;

    template<typename T>
    void update(const T&);

  protected:
    template<typename T>
    void updateValue(T& lhs, const T& rhs) {
        if (rhs) {
            lhs = rhs;
        }
    }

  public:
    std::string toJson() const {
        return StrCat(
                "{\n",
                '\t',   "\"wallTimeMs\": ",         timeStat.wallTimeMs, ",\n",
                '\t',   "\"cpuTimeMs\": ",          timeStat.cpuTimeMs, ",\n",
                '\t',   "\"userTimeMs\": ",         timeStat.userTimeMs, ",\n",
                '\t',   "\"systemTimeMs\": ",       timeStat.systemTimeMs, ",\n",
                '\t',   "\"memoryKb\": ",           memoryKB, ",\n",
                '\t',   "\"rssPeak\": ",            rssPeak, ",\n",
                '\t',   "\"cswVoluntary\": ",       cswVoluntary, ",\n",
                '\t',   "\"cswForced\": ",          cswForced, ",\n",
                '\t',   "\"softPageFaults\": ",     softPageFaults, ",\n",
                '\t',   "\"hardPageFaults\": ",     hardPageFaults, ",\n",
                '\t',   "\"nrSysCalls\": ",         nrSysCalls, ",\n",
                '\t',   "\"lastSysCall\": ",        lastSysCall, ",\n",
                '\t',   "\"terminalSignal\": ",     terminalSignal, ",\n",
                '\t',   "\"exitCode\": ",           exitCode, ",\n",
                '\t',   "\"processWasKilled\": ",   processWasKilled, ",\n",
                '\t',   "\"resultCode\": ",         resultCode, ",\n",
                '\t',   "\"version\": ",            "\"", version, "\"", ",\n",
                '\t',   "\"internalMessage\": ",    "\"", internalMessage, "\"", "\n",
                "}\n"
            );
    }
};

const std::string RunStats::version = "2.0";

template<>
void RunStats::update(const rusage& usage) {
    updateValue(this->rssPeak, usage.ru_maxrss);
    updateValue(this->cswVoluntary, usage.ru_nvcsw);
    updateValue(this->cswForced, usage.ru_nivcsw);
}

template<>
void RunStats::update(const RunStats::ResultCode& resultCode) {
    updateValue(this->resultCode, resultCode);
}

template<>
void RunStats::update(const RunStats::TimeStat& timeStat) {
    updateValue(this->timeStat.wallTimeMs, timeStat.wallTimeMs);
    updateValue(this->timeStat.cpuTimeMs, timeStat.cpuTimeMs);
    updateValue(this->timeStat.userTimeMs, timeStat.userTimeMs);
    updateValue(this->timeStat.systemTimeMs, timeStat.systemTimeMs);
}

}  // namespace tumbletest
