#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
CompileJob::CompileJob(Compilation& compilation)
    : Job(JobType::Compile, compilation) {}

void CompileJob::BuildCmdOutput() {}
