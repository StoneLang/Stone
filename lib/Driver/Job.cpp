#include "stone/Driver/Job.h"

#include "stone/Driver/Compilation.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
Job::Job(JobType jobType, bool isAsync, Compilation &compilation)
    : jobType(jobType), isAsync(isAsync), compilation(compilation) {}

void Job::AddDep(const Job *job) { jobOpts.deps.push_back(job); }

void Job::AddInput(const InputFile input) { jobOpts.inputs.push_back(input); }

const char *Job::GetName(JobType jobType) {
  switch (jobType) {
    case JobType::Compile:
      return "compile";
    case JobType::Backend:
      return "backend";
    case JobType::Assemble:
      return "assemble";
    case JobType::DynamicLink:
      return "dynamic-link";
    case JobType::StaticLink:
      return "static-link";
  }
  llvm_unreachable("invalid class");
}

Job::~Job() {}

void Job::Print(const char *terminator, bool quote, CrashState *crash) const {}

/*
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
