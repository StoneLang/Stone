#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
AssembleJob::AssembleJob(bool isAsync, Compilation& compilation)
    : Job(JobType::Assemble, isAsync, compilation) {}
