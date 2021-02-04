#ifndef STONE_DRIVER_TASKQUEUE_H
#define STONE_DRIVER_TASKQUEUE_H

#include <functional>
#include <memory>
#include <queue>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"
#include "stone/Utils/Context.h"
#include "stone/Utils/LLVM.h"
#include "stone/Utils/Stats.h"

namespace stone {
namespace driver {

class TaskQueue;
class TaskQueueStats final : public Stats {
  const TaskQueue& queue;

 public:
  TaskQueueStats(const TaskQueue& queue)
      : Stats("task-queue statistics:"), queue(queue) {}
  void Print() override;
};

using ProcID = llvm::sys::procid_t;

enum class TaskQueueType { None, Unix, Win };
class TaskQueue {
  friend TaskQueueStats;
  std::unique_ptr<TaskQueueStats> stats;
  TaskQueueType queueType;
  Context& ctx;

  /// Tasks which have not begun execution.
  // std::queue<std::unique_ptr<Task>> queue;
  /// The number of tasks to execute in parallel.
  // unsigned parallelTaskCount;

 public:
  TaskQueue(TaskQueueType queueType, Context& ctx);

 public:
  TaskQueueType GetType() { return queueType; }
  TaskQueueStats& GetStats() { return *stats.get(); }
};

class UnixTaskQueue : public TaskQueue {
 public:
  UnixTaskQueue(Context& ctx) : TaskQueue(TaskQueueType::Unix, ctx) {}
};

class WinTaskQueue : public TaskQueue {
 public:
  WinTaskQueue(Context& ctx) : TaskQueue(TaskQueueType::Win, ctx) {}
};

}  // namespace driver
}  // namespace stone
#endif
