#ifndef STONE_CORE_STATS_H
#define STONE_CORE_STATS_H

#include <iostream>

#include "stone/Utils/Color.h"
#include "stone/Utils/List.h"
#include "stone/Utils/Mem.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"
namespace stone {

class StatsPrinter {};
class StatsListener {};

class Stats {
  bool enabled = false;
  ConstList<Stats> deps;

protected:
  const char *name = nullptr;
  ColorOutputStream cos;

public:
  std::unique_ptr<llvm::Timer> timer;

public:
  Stats(const char *name);
  virtual ~Stats() {}
  const char *GetName() const { return name; }
  void Enable() { enabled = true; }
  void Disable() { enabled = false; }
  void AddDep(const Stats *stats) { deps.Add(stats); }
  ConstList<Stats> GetDeps() { return deps; }

  llvm::Timer &GetTimer() { return *timer.get(); }

public:
  virtual void Print() = 0;
};

class StatEngine final {
  llvm::SmallVector<Stats *, 4> entries;

public:
  StatEngine();
  ~StatEngine();

public:
  void Register(Stats *stats);
  /// Print all groups and entries in groups
  void Print();
};
} // namespace stone

#endif
