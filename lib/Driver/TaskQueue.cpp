#include "stone/Driver/TaskQueue.h"

using namespace stone;
using namespace stone::driver;

TaskQueue::TaskQueue(TaskQueueType queueType, Context &ctx)
    : queueType(queueType), ctx(ctx) {
  stats.reset(new TaskQueueStats(*this, ctx));
  ctx.GetStatEngine().Register(stats.get());
}

void TaskQueueStats::Print() {}
