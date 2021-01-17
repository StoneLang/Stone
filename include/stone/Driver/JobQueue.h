#ifndef STONE_DRIVER_JOBQUEUE_H
#define STONE_DRIVER_JOBQUEUE_H

#include "llvm/ADT/PriorityQueue.h"
#include "stone/Driver/Job.h"

namespace stone {
namespace driver {
class Job;
class JobQueue final {
  llvm::PriorityQueue<Job *> queue;

 public:
  JobQueue();
  ~JobQueue();
};
}  // namespace driver
}  // namespace stone
#endif
