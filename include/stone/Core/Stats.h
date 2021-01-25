#ifndef STONE_CORE_STATS_H
#define STONE_CORE_STATS_H

#include <iostream>

#include "stone/Core/Mem.h"

namespace stone {

class StatsPrinter {};
class StatsListener {};

class Stats {
 protected:
  llvm::raw_ostream &os;

 public:
  Stats() : os(llvm::outs()) {}
  virtual ~Stats() {}

 public:
  virtual void Print() const = 0;
};

class StatEngine final {
 public:
  StatEngine();
  ~StatEngine();

  /// Owns the Stats
  void Add(std::unique_ptr<Stats> stats);
  ///
  void Print();
};
}  // namespace stone

#endif
