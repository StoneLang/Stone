#include "stone/Utils/Stats.h"

using namespace stone;

Stats::Stats(const char *name) : name(name), cos(llvm::outs()) {
  timer.startTimer();
}

StatEngine::StatEngine() {}
StatEngine::~StatEngine() {}

void StatEngine::Register(Stats *stats) {
  assert(stats && "Unable to register null stats.");
  entries.push_back(stats);
}

void StatEngine::Print() {
  for (const auto &stats : entries) {
    stats->Print();
  }
}
