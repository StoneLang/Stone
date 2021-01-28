#include "stone/Driver/CompileJob.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
CompileJob::CompileJob(bool isAsync, Compilation& compilation)
    : Job(JobType::Compile, isAsync, compilation) {}

void CompileJob::BuildOutputProfile() {}
