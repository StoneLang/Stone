#ifndef STONE_DRIVER_JOB_H
#define STONE_DRIVER_JOB_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "stone/Core/List.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/JobOptions.h"
#include "stone/Driver/JobType.h"
#include "stone/Driver/LinkType.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

class Job;
class Compilation;

class Job {
  JobType jobType;
  JobOptions jobOpts;
  Compilation& compilation;

 public:
  Job(JobType jobType, Compilation& compilation);

 public:
  JobType GetType() const { return jobType; }
  Jobs& GetDeps() { return jobOpts.deps; }

  void AddInput(const InputFile input);
  void AddDep(const Job* job);

  const JobOptions& GetJobOptions() const { return jobOpts; }
  Compilation& GetCompilation() { return compilation; }

  void Print(const char* terminator, bool quote, CrashState* crashState);

 private:
  friend class Compilation;
  /// Jobs are creaed through Compilation::CreateJob
  void* operator new(size_t size) { return ::operator new(size); };
};

class CompileJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  CompileJob(Compilation& compilation) : Job(JobType::Compile, compilation) {}

 public:
  static bool classof(const Job* j) { return j->GetType() == JobType::Compile; }
};

class LinkJob : public Job {
  LinkType linkType;

 public:
  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, Compilation& compilation, LinkType linkType)
      : Job(jobType, compilation), linkType(linkType) {}

 public:
  LinkType GetLinkType() { return linkType; }
};
class StaticLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(Compilation& compilation, LinkType linkType)
      : LinkJob(JobType::StaticLink, compilation, linkType) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::StaticLink;
  }
};

class DynamicLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(Compilation& compilation, LinkType linkType)
      : LinkJob(JobType::DynamicLink, compilation, linkType) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::DynamicLink;
  }
};

class AssembleJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  AssembleJob(Compilation& compilation) : Job(JobType::Assemble, compilation) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::Assemble;
  }
};

class BackendJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  BackendJob(Compilation& ctx) : Job(JobType::Backend, ctx) {}

 public:
  static bool classof(const Job* j) { return j->GetType() == JobType::Backend; }
};

// This is used in compilation where they are stored and shared
class SafeJobs final : public List<Job> {
 public:
  void Print() const;
};
}  // namespace driver
}  // namespace stone
#endif
