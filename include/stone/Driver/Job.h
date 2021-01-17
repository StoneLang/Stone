#ifndef STONE_DRIVER_JOB_H
#define STONE_DRIVER_JOB_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/StringSaver.h"
#include "stone/Core/List.h"
#include "stone/Driver/Activity.h"
#include "stone/Driver/CrashCondition.h"

namespace stone {
namespace driver {

class Driver;
// class JobOutput {
//};

class Job {
  friend class Compilation;

  Driver &driver;
  CompilationActivity &trigger;
  /// The input file list in case we need to emit a file list instead of a
  /// proper response file
  llvm::opt::ArgStringList inputFiles;

 public:
  Job(CompilationActivity &trigger, Driver &driver)
      : trigger(trigger), driver(driver) {}
  virtual ~Job();

 public:
  CompilationActivity &GetTrigger() { return trigger; }

  virtual void Print(llvm::raw_ostream &os, const char *terminator, bool quote,
                     CrashCondition *crash = nullptr) const;

  virtual int AsyncExecute(
      llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects,
      std::string *errMsg, bool *failed) const;

  virtual int SyncExecute(
      llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects,
      std::string *errMsg, bool *failed) const;

 public:
  // llvm::SmallVector<Job *, 10> deps;
  //
  /// Set an input file list, necessary if you specified an RF_FileList response
  /// file support.
  void SetInputFiles(llvm::opt::ArgStringList files) {
    inputFiles = std::move(files);
  }
};

class SyncJob : public Job {};
class AsyncJob : public Job {};

class JobList final : public List<Job> {
 public:
  void Print() const;
};

}  // namespace driver
}  // namespace stone
#endif
