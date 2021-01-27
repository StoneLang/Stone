#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
Job::Job(JobType jobType, Context& ctx) : jobType(jobType), ctx(ctx){
}

void Job::AddDep(const Job* job) { jobOpts.deps.push_back(job); }

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
