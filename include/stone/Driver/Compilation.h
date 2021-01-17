#ifndef STONE_DRIVER_COMPILATION_H
#define STONE_DRIVER_COMPILATION_H

#include <cassert>
#include <iterator>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include "stone/Core/LLVM.h"
#include "stone/Driver/Activity.h"
#include "stone/Driver/JobQueue.h"

namespace llvm {
namespace opt {
class DerivedArgList;
class InputArgList;
}  // namespace opt
}  // namespace llvm

namespace stone {
namespace driver {

class ToolChain;
class Driver;

class Compilation final {
  /// The System we were created by.
  const Driver &driver;

  /// Temporary files which should be removed on exit.
  llvm::opt::ArgStringList tempFiles;

  /// True if temporary files should not be deleted.
  const bool saveTempFiles = false;

  /// Result files which should be removed on failure.
  // ArgStringMap resultFiles;

  /// Result files which are generated correctly on failure, and which should
  /// only be removed if we crash.
  // ArgStringMap failureResultFiles;

  /// A list of all of the activities
  ActivityList activities;

  /// A list of all the procs
  JobList jobs;

 public:
  Compilation(Driver &driver);
  ~Compilation();

 public:
  /// Creates a new Activity owned by this Compilation.
  ///
  /// The new Activity is *not* added to the list returned by GetActivitys().
  template <typename T, typename... Args>
  T *CreateActivity(Args &&...arg) {
    auto event = new T(std::forward<Args>(arg)...);
    activities.Add(std::unique_ptr<stone::driver::Activity>(event));
    return event;
  }

  ActivityList &GetActivitys() { return activities; }
  const ActivityList &GetActivitys() const { return activities; }

  JobList &GetJobs() { return jobs; }
  const JobList &GetJobs() const { return jobs; }

  void AddJob(std::unique_ptr<Job> job) { jobs.Add(std::move(job)); }

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
      const JobList &jobss,
      llvm::SmallVectorImpl<std::pair<int, const Job *>> &fallBackProcs) const;
};
}  // namespace driver
}  // namespace stone
#endif
