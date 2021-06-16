#ifndef STONE_CORE_STATS_H
#define STONE_CORE_STATS_H

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Mem.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/Timer.h"

#include <iostream>

namespace stone {
class Context;
class StatsPrinter {};
class StatsListener {};

class Stats {
  bool enabled = false;
  ConstList<Stats> deps;
  Context &ctx;

protected:
  const char *name = nullptr;

public:
  std::unique_ptr<llvm::Timer> timer;

public:
  Stats(const char *name, Context &ctx);
  virtual ~Stats() {}

public:
  const char *GetName() const { return name; }
  void Enable() { enabled = true; }
  void Disable() { enabled = false; }
  void AddDep(const Stats *stats) { deps.Add(stats); }
  ConstList<Stats> GetDeps() { return deps; }

  llvm::Timer &GetTimer() { return *timer.get(); }

  Context &GetContext() { return ctx; }

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
