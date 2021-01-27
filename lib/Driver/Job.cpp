#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
Job::Job(JobType jobType, Context& ctx, InputFiles* inputs,
         OutputFileType output)
    : jobType(jobType), ctx(ctx), inputs(inputs) {
  jobOpts.outputFileType = output;
}

// Some jobs only consume inputs -- For example, LinkJob
Job::Job(JobType jobType, Context& ctx, Jobs deps, OutputFileType output)
    : jobType(jobType), ctx(ctx), deps(deps) {
  jobOpts.outputFileType = output;
}

/*
const char *Job::GetName(JobType jobType) {
}

*/

/*
Job::~Job() {}

void Job::Print(llvm::raw_ostream &os, const char *terminator, bool quote,
                CrashCondition *crash) const {}

int Job::AsyncExecute(llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects,
                      std::string *errMsg, bool *failed) const {
  return 0;
}

int Job::SyncExecute(llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects,
                     std::string *errMsg, bool *failed) const {
  return 0;
}

*/

void SafeJobs::Print() const {}
