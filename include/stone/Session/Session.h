#ifndef STONE_SESSION_SESSION_H
#define STONE_SESSION_SESSION_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptSpecifier.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "stone/Core/Context.h"
#include "stone/Session/FileType.h"
#include "stone/Session/Mode.h"
#include "stone/Session/SessionOptions.h"

namespace stone {

/// An input argument from the command line and its inferred type.
using InputPair = std::pair<file::FileType, const llvm::opt::Arg *>;

/// Type used for a list of input arguments.
using InputFiles = llvm::SmallVector<InputPair, 16>;

enum class SessionKind { Compiler, Driver };

class Session : public Context {
  /// The mode id for this session
  SessionOptions &sessionOpts;
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fileSystem;

 protected:
  Mode mode;
  /// Bit flags for OptTable
  unsigned includedFlagsBitmask = 0;
  /// This bit flag will ensure the correct mode for the session
  unsigned excludedFlagsBitmask = 0;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  /// Default target triple.
  std::string targetTriple;

  /// The name of the module which we are building.
  std::string moduleName;

  /// The original (untranslated) input argument list.
  std::unique_ptr<llvm::opt::InputArgList> originalArgs;

  /// The driver translated arguments. Note that toolchains may perform their
  /// own argument translation.
  std::unique_ptr<llvm::opt::DerivedArgList> translatedArgs;

  /// Allocator for Compiles
  mutable llvm::BumpPtrAllocator bumpAlloc;

  /// Object that stores strings read from configuration file.
  llvm::StringSaver strSaver;

 public:
  void SetFS(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> fs) {
    fileSystem = fs;
  }
  llvm::vfs::FileSystem &GetFS() const { return *fileSystem; }
  //
  /// When the session was started.
  ///
  /// This should be as close as possible to when the driver was invoked, since
  /// it's used as a lower bound.
  llvm::sys::TimePoint<> startTime;

  /// The time of the last build.
  ///
  /// If unknown, this will be some time in the past.
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

 public:
  Session(SessionOptions &sessionOpts);
  ~Session();

 protected:
  std::unique_ptr<llvm::opt::InputArgList> BuildArgList(
      llvm::ArrayRef<const char *> args);

 public:
  ///
  virtual bool Build(llvm::ArrayRef<const char *> args) = 0;
  ///
  virtual int Run() = 0;
  ///
  virtual void PrintLifecycle() = 0;
  ///
  virtual void PrintHelp(bool showHidden) = 0;

  void SetTargetTriple(const llvm::Triple &triple);

  void SetTargetTriple(llvm::StringRef triple);

  llvm::StringRef GetTargetTriple() const { return langOpts.target.str(); }

  // virtual llvm::StringRef GetName() = 0;
  void Finish();
  void PrintDiagnostics();
  void PrintStatistics();

  Mode &GetMode();

  /// The original (untranslated) input argument list.
  llvm::opt::InputArgList &GetOriginalArgs() { return *originalArgs.get(); }

  /// The driver translated arguments. Note that toolchains may perform their
  /// own argument translation.
  llvm::opt::DerivedArgList &GetTranslatedArgs() {
    return *translatedArgs.get();
  }
  std::string GetModuleName() { return moduleName; }

  // TODO: You want to say
  void AddFile();

  /// Return the total amount of physical memory allocated
  /// for representing CompileInstances
  size_t GetMemSize() const { return bumpAlloc.getTotalMemory(); }

 protected:
  // Compute the mode id -- TODO: virtual
  virtual void ComputeMode(const llvm::opt::DerivedArgList &args);
  /// TranslateInputArgs - Create a new derived argument list from the input
  /// arguments, after applying the standard argument translations.
  virtual llvm::opt::DerivedArgList *TranslateInputArgs(
      const llvm::opt::InputArgList &args) const;

  virtual ModeKind GetDefaultModeKind() = 0;

  void Purge();
};

}  // namespace stone
#endif
