#ifndef STONE_DRIVER_JOB_H
#define STONE_DRIVER_JOB_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "stone/Core/List.h"
#include "stone/Core/Stats.h"
#include "stone/Driver/CmdOutput.h"
#include "stone/Driver/CrashState.h"
#include "stone/Driver/JobOptions.h"
#include "stone/Driver/JobProfile.h"
#include "stone/Driver/JobType.h"
#include "stone/Driver/LinkType.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {
class Job;
class Compilation;
using JobID = int64_t;
using Jobs = llvm::SmallVector<const Job*, 4>;

class JobStats final : public Stats {
  const Job& job;

 public:
  JobStats(const Job& compilation) : Stats("job statistics:"), job(job) {}
  void Print() override;
};

class Job {
  friend JobStats;

 protected:
  bool isAsync;
  JobType jobType;
  JobOptions jobOpts;
  Compilation& compilation;
  JobID jobID;
  /// Inputs
  Jobs deps;
  /// The output of this command.
  std::unique_ptr<CmdOutput> cmdOutput;

  std::unique_ptr<JobStats> stats;

  /// The executable to run.
  const char* executable = nullptr;

  /// The list of program arguments (not including the implicit first argument,
  /// which will be the Executable).
  ///
  /// These argument strings must be kept alive as long as the Job is alive.
  llvm::opt::ArgStringList arguments;

 public:
  Job(JobType jobType, bool isAsync, Compilation& compilation);
  virtual ~Job();

 public:
  JobType GetType() const { return jobType; }
  Jobs& GetDeps() { return deps; }

  void AddInput(const InputFile input);
  void AddDep(const Job* job);
  void AddArgument();

  const JobOptions& GetJobOptions() const { return jobOpts; }
  Compilation& GetCompilation() { return compilation; }
  bool IsAsync() { return isAsync; }

  void Print(const char* terminator, bool quote, CrashState* crashState) const;

  const char* GetName() { return Job::GetNameByType(jobType); }

  llvm::StringRef GetOutputTypeName() {
    return file::GetTypeName(jobOpts.outputFileType);
  }

  /// TODO
  void SetJobID(JobID jid) { jobID = jid; }
  JobID GetJobID() { return jobID; }

  // TODO: Think about
  virtual void BuildCmdOutput() = 0;

  // TODO: Think about
  void SetCmdOutput(std::unique_ptr<CmdOutput> output) {
    cmdOutput = std::move(output);
  }
  void SetExecutable(const char* exec) { executable = exec; }

  JobStats& GetStats() { return *stats.get(); }

 public:
  static const char* GetNameByType(JobType jobType);

 private:
  friend class Compilation;
  /// Jobs are creaed through Compilation::CreateJob
  void* operator new(size_t size) { return ::operator new(size); };
};

class AssembleJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  AssembleJob(bool isAsync, Compilation& compilation);

  void BuildCmdOutput() override;

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::Assemble;
  }
};

class BackendJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  BackendJob(bool isAsync, Compilation& compilation);

  void BuildCmdOutput() override;

 public:
  static bool classof(const Job* j) { return j->GetType() == JobType::Backend; }
};

class CompileJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  CompileJob(bool isAsync, Compilation& compilation);

  void BuildCmdOutput() override;

 public:
  static bool classof(const Job* j) { return j->GetType() == JobType::Compile; }
};

class LinkJob : public Job {
  LinkType linkType;
  bool requiresLTO;

 public:
  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, bool isAsync, Compilation& compilation,
          bool requiresLTO, LinkType linkType);

  virtual void BuildCmdOutput() = 0;

 public:
  LinkType GetLinkType() { return linkType; }
  bool RequiresLTO() { return requiresLTO; }
};

class DynamicLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(bool isAsync, Compilation& compilation, bool requiresLTO,
                 LinkType linkType);

  void BuildCmdOutput() override;

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::DynamicLink;
  }
};

class StaticLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(bool isAsync, Compilation& compilation, bool requiresLTO,
                LinkType linkType);

  void BuildCmdOutput() override;

 public:
  static bool classof(const Job* j) {
    return j->GetType() == JobType::StaticLink;
  }
};

}  // namespace driver
}  // namespace stone
#endif
