#ifndef STONE_DRIVER_TASKQUEUE_H
#define STONE_DRIVER_TASKQUEUE_H

#include <functional>
#include <memory>
#include <queue>

#include "stone/Basic/Basic.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Stats.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {
namespace driver {

class TaskQueue;
class TaskQueueStats final : public Stats {
  const TaskQueue &queue;

public:
  TaskQueueStats(const TaskQueue &queue, Basic &basic)
      : Stats("task-queue statistics:", basic), queue(queue) {}
  void Print() override;
};

using ProcID = llvm::sys::procid_t;

enum class TaskQueueType { None, Unix, Win };
class TaskQueue {
  friend TaskQueueStats;
  std::unique_ptr<TaskQueueStats> stats;
  TaskQueueType queueType;
  Basic &basic;

  /// Tasks which have not begun execution.
  // std::queue<std::unique_ptr<Task>> queue;
  /// The number of tasks to execute in parallel.
  // unsigned parallelTaskCount;

public:
  TaskQueue(TaskQueueType queueType, Basic &basic);

public:
  TaskQueueType GetType() { return queueType; }
  TaskQueueStats &GetStats() { return *stats.get(); }
};

class UnixTaskQueue : public TaskQueue {
public:
  UnixTaskQueue(Basic &basic) : TaskQueue(TaskQueueType::Unix, basic) {}
};

class WinTaskQueue : public TaskQueue {
public:
  WinTaskQueue(Basic &basic) : TaskQueue(TaskQueueType::Win, basic) {}
};

} // namespace driver
} // namespace stone
#endif
