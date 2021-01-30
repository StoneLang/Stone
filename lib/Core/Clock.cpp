#include "stone/Core/Clock.h"

using namespace stone;

void Clock::Start() { startTime = std::chrono::system_clock::now(); }
void Clock::Stop() { endTime = std::chrono::system_clock::now(); }

void Clock::PrintMicroSeconds() {
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime)
          .count();
  llvm::outs() << "duration (microseconds): " << duration << '\n';
}

void Clock::PrintMilliSeconds() {
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  llvm::outs() << "duration (milliseconds): " << duration << '\n';
}

void Clock::PrintSeconds() {
  auto duration =
      std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime)
          .count();
  llvm::outs() << "duration (seconds): " << duration << '\n';
}

void Clock::PrintMinutes() {
  auto duration =
      std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime)
          .count();
  llvm::outs() << "duration (minutes): " << duration << '\n';
}

void Clock::Print() {
  switch (timeType) {
    case TimeType::Microseconds:
      PrintMicroSeconds();
      break;
    case TimeType::Milliseconds:
      PrintMilliSeconds();
      break;
    case TimeType::Seconds:
      PrintSeconds();
      break;
    case TimeType::Minutes:
      PrintMinutes();
      break;
    default:
      break;
  }
}
