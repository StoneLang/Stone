#include "stone/Driver/TaskQueue.h"

using namespace stone;
using namespace stone::driver;

TaskQueue::TaskQueue(TaskQueueType queueType, Basic &basic)
    : queueType(queueType), basic(basic) {
  stats.reset(new TaskQueueStats(*this, basic));
  basic.GetStatEngine().Register(stats.get());
}

void TaskQueueStats::Print() {}
