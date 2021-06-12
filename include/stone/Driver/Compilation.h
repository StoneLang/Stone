#ifndef STONE_DRIVER_COMPILATION_H
#define STONE_DRIVER_COMPILATION_H

#include <cassert>
#include <iterator>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "stone/Basic/Clock.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Stats.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/TaskQueue.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"

namespace llvm {
namespace opt {
class DerivedArgList;
class InputArgList;
} // namespace opt
} // namespace llvm

namespace stone {
namespace driver {

class Driver;
class ToolChain;
class Compilation;
using ArgStringMap = llvm::DenseMap<const Job *, const char *>;

class Compilation;
class CompilationStats final : public Stats {
  const Compilation &compilation;

public:
  CompilationStats(const Compilation &compilation)
      : Stats("compilation statistics:"), compilation(compilation) {}
  void Print() override;
};

class CompilationResult {};
class Compilation final {
  friend CompilationStats;

  Driver &driver;

  Clock clock;

  std::unique_ptr<CompilationStats> stats;

  std::unique_ptr<driver::TaskQueue> queue;

  /// A list of all the top-level jobs to run
  ConstList<Job> jobs;

  /// Temporary files which should be removed on exit.
  llvm::opt::ArgStringList tempFiles;

  /// True if temporary files should not be deleted.
  const bool saveTempFiles = false;

  /// Result files which should be removed on failure.
  ArgStringMap resultFiles;

  /// Result files which are generated correctly on failure, and which should
  /// only be removed if we crash.
  ArgStringMap failureResultFiles;

public:
  Compilation(Driver &driver);
  ~Compilation();

public:
  void AddJob(const Job *job);

  TaskQueue &GetQueue() { return *queue.get(); }
  const TaskQueue &GetQueue() const { return *queue.get(); }

  ConstList<Job> &GetJobs() { return jobs; }
  const ConstList<Job> &GetJobs() const { return jobs; }

  CompilationStats &GetStats() { return *stats.get(); }

  const Driver &GetDriver() const { return driver; }
  Driver &GetDriver() { return driver; }

  /// addTempFile - Add a file to remove on exit, and returns its
  /// argument.
  const char *AddTempFile(const char *name) {
    tempFiles.push_back(name);
    return name;
  }

  /// PurgeFile - Delete a given file.
  ///
  /// \param IssueErrors - Report failures as errors.
  /// \return Whether the file was removed successfully.
  bool PurgeFile(const char *name, bool issueErrors = false) const;

  /// PurgeFiles - Remove the files in the given list.
  ///
  /// \param IssueErrors - Report failures as errors.
  /// \return Whether all files were removed successfully.
  bool PurgeFiles(const llvm::opt::ArgStringList &files,
                  bool issueErrors = false) const;

  // bool ExecuteProcs(std::unique_ptr<stone::JobQueue> &&queue);
  /// ExecuteProc - Execute an actual command.
  ///
  /// \param fallBackProc - For non-zero results, this will be set to the
  /// Command which failed, if any.
  /// \return The result code of the subprocess.
  int ExecuteJob(const Job &job, const Job *&fallBackJob) const;

  /// ExecuteProc - Execute a single job.
  ///
  /// \param fallBackProc - For non-zero results, this will be a vector of
  /// failing commands and their associated result code.
  void ExecuteJobs(
      llvm::SmallVectorImpl<std::pair<int, const Job *>> &fallBackJob) const;

public:
  /// Asks the Compilation to perform the Jobs which it knows about.
  ///
  /// \param TQ The TaskQueue used to schedule jobs for execution.
  ///
  /// \returns result code for the Compilation's Jobs; 0 indicates success and
  /// -2 indicates that one of the Compilation's Jobs crashed during execution
  CompilationResult Run(std::unique_ptr<driver::TaskQueue> &&queue);
};
} // namespace driver
} // namespace stone
#endif
