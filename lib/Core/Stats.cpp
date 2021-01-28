#include "stone/Core/Stats.h"

using namespace stone;

StatEngine::StatEngine() {}
StatEngine::~StatEngine() {}

void StatEngine::Register(const Stats* stats) {
  assert(stats && "Unable to register null stats.");
  entries.push_back(stats);
}

void StatEngine::Print() {
  for (const auto& stats : entries) {
    stats->Print();
  }
}
