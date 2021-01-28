#ifndef STONE_CORE_STATS_H
#define STONE_CORE_STATS_H

#include <iostream>

#include "llvm/ADT/SmallVector.h"
#include "stone/Core/Color.h"
#include "stone/Core/List.h"
#include "stone/Core/Mem.h"

namespace stone {

class StatsPrinter {};
class StatsListener {};

class Stats {
  bool enabled = false;

 protected:
  const char* name = nullptr;
  ColorOutputStream cos;

 public:
  Stats(const char* name);
  virtual ~Stats() {}
  const char* GetName() const { return name; }
  void Enable() { enabled = true; }
  void Disable() { enabled = false; }

 public:
  virtual void Print() const = 0;
};

class StatEngine final {
  llvm::SmallVector<const Stats*, 4> entries;

 public:
  StatEngine();
  ~StatEngine();

 public:
  void Register(const Stats* stats);
  /// Print all groups and entries in groups
  void Print();
};
}  // namespace stone

#endif
