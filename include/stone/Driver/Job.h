#ifndef STONE_DRIVER_JOB_H
#define STONE_DRIVER_JOB_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "stone/Core/List.h"
#include "stone/Core/Context.h"
#include "stone/Driver/JobOptions.h"
#include "stone/Driver/JobType.h"
#include "stone/Driver/LinkType.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

class Job;
class Driver;


class Job {
  JobType jobType;
  JobOptions jobOpts;
  Context& ctx;

 public:
  // Job(JobType jobType);
  // Some job depend on other jobs -- For example, LinkJob
  Job(JobType jobType, Context& ctx);

 public:
  JobType GetType() const { return jobType; }

  Jobs& GetDeps() { return deps; }
  void AddDep(const Job* job) { deps.push_back(job); }

  const JobOptions& GetJobOptions() const { return jobOpts; }
  Context& GetContext() { return ctx; }

 private:
  friend class Compilation;
  /// Jobs are creaed through Compilation::CreateJob
  void* operator new(size_t size) { return ::operator new(size); };
};

class CompileJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  CompileJob(Context& ctx)
      : Job(JobType::Compile, ctx, inputs, output) {}

 public:
  static bool classof(const Job* j) { return j->GetType() == JobType::Compile; }
};

class LinkJob : public Job {
  LinkType linkType;

 public:
  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, Context& ctx, InputFiles* inputs, LinkType linkType,
          OutputFileType output)
      : Job(jobType, ctx, inputs, output), linkType(linkType) {}

  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, Context& ctx, Jobs deps, LinkType linkType,
          OutputFileType output)
      : Job(JobType::StaticLink, ctx, deps, output), linkType(linkType) {}

 public:
  LinkType GetLinkType() { return linkType; }
};
class StaticLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(Context& ctx, InputFiles* inputs, LinkType linkType,
                OutputFileType output)
      : LinkJob(JobType::StaticLink, ctx, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(Context& ctx, Jobs deps, LinkType linkType,
                OutputFileType output)
      : LinkJob(JobType::StaticLink, ctx, deps, linkType, output) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::StaticLink;
  }
};

class DynamicLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(Context& ctx, InputFiles* inputs, LinkType linkType,
                 OutputFileType output)
      : LinkJob(JobType::DynamicLink, ctx, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(Context& ctx, Jobs deps, LinkType linkType,
                 OutputFileType output)
      : LinkJob(JobType::DynamicLink, ctx, deps, linkType, output) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::DynamicLink;
  }
};

class AssembleJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  AssembleJob(Context& ctx, InputFiles* inputs, OutputFileType output)
      : Job(JobType::Assemble, ctx, inputs, output) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::Assemble;
  }
};

class BackendJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  BackendJob(Context& ctx, InputFiles* inputs, OutputFileType output)
      : Job(JobType::Backend, ctx, inputs, output) {}

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
