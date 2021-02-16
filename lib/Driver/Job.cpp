#include "stone/Driver/Job.h"

#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
Job::Job(JobType jobType, Compilation &compilation)
    : jobType(jobType), jobID(0), isAsync(true), compilation(compilation) {

  // TODO: if -print-stats
  timer.startTimer();

  stats.reset(new JobStats(*this));
  compilation.GetDriver().GetStatEngine().Register(stats.get());
}

void Job::AddDep(const Job *job) { deps.Add(job); }

void Job::AddInput(const File input) { jobOpts.inputs.push_back(input); }

const char *Job::GetNameByType(JobType jobType) {
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

void JobStats::Print() {}

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
