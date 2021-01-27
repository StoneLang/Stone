#ifndef STONE_DRIVER_JOB_H
#define STONE_DRIVER_JOB_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "stone/Core/List.h"
#include "stone/Driver/JobOptions.h"
#include "stone/Driver/JobType.h"
#include "stone/Driver/LinkType.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

class Job;
class Driver;

using OutputFileType = file::FileType;
using Jobs = llvm::SmallVector<const Job*, 4>;

class Job {
  Jobs deps;
  JobType jobType;
  InputFiles* inputs;
  OutputFileType output;
  const JobOptions& jobOpts;

 public:
  // Some job depend on other jobs -- For example, LinkJob
  Job(JobType jobType, const JobOptions& jobOpts, InputFiles* inputs,
      OutputFileType output)
      : jobType(jobType), jobOpts(jobOpts), inputs(inputs), output(output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  Job(JobType jobType, const JobOptions& jobOpts, Jobs deps,
      OutputFileType output)
      : jobType(jobType), jobOpts(jobOpts), deps(deps), output(output) {}

  JobType GetType() const { return jobType; }
  Jobs& GetDeps() { return deps; }
  InputFiles& GetInputs() { return *inputs; }
  OutputFileType GetOutputFileType() { return output; }
  const JobOptions& GetJobOptions() const { return jobOpts; }

 private:
  friend class Compilation;
  /// Jobs are creaed through Compilation::CreateJob
  void* operator new(size_t size) { return ::operator new(size); };
};

class CompileJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  CompileJob(const JobOptions& jobOpts, InputFiles* inputs,
             OutputFileType output)
      : Job(JobType::Compile, jobOpts, inputs, output) {}

 public:
  static bool classof(const Job* j) { return j->GetType() == JobType::Compile; }
};

class LinkJob : public Job {
  LinkType linkType;

 public:
  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, const JobOptions& jobOpts, InputFiles* inputs,
          LinkType linkType, OutputFileType output)
      : Job(jobType, jobOpts, inputs, output), linkType(linkType) {}

  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, const JobOptions& jobOpts, Jobs deps,
          LinkType linkType, OutputFileType output)
      : Job(JobType::StaticLink, jobOpts, deps, output), linkType(linkType) {}

 public:
  LinkType GetLinkType() { return linkType; }
};
class StaticLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(const JobOptions& jobOpts, InputFiles* inputs,
                LinkType linkType, OutputFileType output)
      : LinkJob(JobType::StaticLink, jobOpts, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(const JobOptions& jobOpts, Jobs deps, LinkType linkType,
                OutputFileType output)
      : LinkJob(JobType::StaticLink, jobOpts, deps, linkType, output) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::StaticLink;
  }
};

class DynamicLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(const JobOptions& jobOpts, InputFiles* inputs,
                 LinkType linkType, OutputFileType output)
      : LinkJob(JobType::DynamicLink, jobOpts, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(const JobOptions& jobOpts, Jobs deps, LinkType linkType,
                 OutputFileType output)
      : LinkJob(JobType::DynamicLink, jobOpts, deps, linkType, output) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::DynamicLink;
  }
};

class AssembleJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  AssembleJob(const JobOptions& jobOpts, InputFiles* inputs,
              OutputFileType output)
      : Job(JobType::Assemble, jobOpts, inputs, output) {}

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::Assemble;
  }
};

class BackendJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  BackendJob(const JobOptions& jobOpts, InputFiles* inputs,
             OutputFileType output)
      : Job(JobType::Backend, jobOpts, inputs, output) {}

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
