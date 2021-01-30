#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
BackendJob::BackendJob(Compilation& compilation)
    : Job(JobType::Backend, compilation) {}
