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
  JobOptions& jobOpts;

 public:
  // Some job depend on other jobs -- For example, LinkJob
  Job(JobType jobType, JobOptions& jobOpts, InputFiles* inputs,
      OutputFileType output)
      : jobType(jobType), jobOpts(jobOpts), inputs(inputs), output(output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  Job(JobType jobType, JobOptions& jobOpts, Jobs deps, OutputFileType output)
      : jobType(jobType), jobOpts(jobOpts), deps(deps), output(output) {}

  JobType GetType() const { return jobType; }
  Jobs& GetDeps() { return deps; }
  InputFiles& GetInputs() { return *inputs; }
  OutputFileType GetOutputFileType() { return output; }

  JobOptions& GetJobOptions() { return jobOpts; }
  const JobOptions& GetJobOptions() const { return jobOpts; }
};

class CompileJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  CompileJob(JobOptions& jobOpts, InputFiles* inputs, OutputFileType output)
      : Job(JobType::Compile, jobOpts, inputs, output) {}
};

class LinkJob : public Job {
  LinkType linkType;

 public:
  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, JobOptions& jobOpts, InputFiles* inputs,
          LinkType linkType, OutputFileType output)
      : Job(jobType, jobOpts, inputs, output), linkType(linkType) {}

  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, JobOptions& jobOpts, Jobs deps, LinkType linkType,
          OutputFileType output)
      : Job(JobType::StaticLink, jobOpts, deps, output), linkType(linkType) {}

 public:
  LinkType GetLinkType() { return linkType; }
};
class StaticLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(JobOptions& jobOpts, InputFiles* inputs, LinkType linkType,
                OutputFileType output)
      : LinkJob(JobType::StaticLink, jobOpts, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(JobOptions& jobOpts, Jobs deps, LinkType linkType,
                OutputFileType output)
      : LinkJob(JobType::StaticLink, jobOpts, deps, linkType, output) {}
};

class DynamicLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(JobOptions& jobOpts, InputFiles* inputs, LinkType linkType,
                 OutputFileType output)
      : LinkJob(JobType::DynamicLink, jobOpts, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(JobOptions& jobOpts, Jobs deps, LinkType linkType,
                 OutputFileType output)
      : LinkJob(JobType::DynamicLink, jobOpts, deps, linkType, output) {}
};

// This is used in compilation where they are stored and shared
class SafeJobs final : public List<Job> {
 public:
  void Print() const;
};

}  // namespace driver
}  // namespace stone
#endif
