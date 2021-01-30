#ifndef STONE_CORE_CLOCK_H
#define STONE_CORE_CLOCK_H

#include <iostream>

#include "llvm/Support/Chrono.h"

namespace stone {

class Clock final {
 public:
  enum class TimeType {
    None,
    Seconds,
    Minutes,
    Microseconds,
    Milliseconds,
  };
  TimeType timeType = TimeType::Milliseconds;

 private:
  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

 public:
  void PrintSeconds();
  void PrintMinutes();
  void PrintMilliSeconds();
  void PrintMicroSeconds();

 public:
  Clock() {}
  void Start();
  void Stop();
  void Print();
  llvm::sys::TimePoint<> GetDuration();
};

}  // namespace stone
#endif
