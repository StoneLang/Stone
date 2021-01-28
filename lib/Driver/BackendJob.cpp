#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
BackendJob::BackendJob(bool isAsync, Compilation& compilation)
    : Job(JobType::Backend, isAsync, compilation) {}
