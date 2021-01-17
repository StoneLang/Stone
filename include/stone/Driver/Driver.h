#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "stone/Core/Mem.h"
#include "stone/Driver/Activity.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Session/Session.h"

using namespace llvm::opt;
namespace llvm {
class Triple;
namespace vfs {
class FileSystem;
}
namespace opt {
class Arg;
class ArgList;
class OptTable;
class InputArgList;
class DerivedArgList;
}  // namespace opt
}  // namespace llvm

namespace stone {
namespace driver {

class Activity;
class Job;
class Compilation;
class ToolChain;

enum class CompileType {
  None,
  /// Multiple compile invocations and -main-file.
  MultipleInvocation,
  /// A compilation using a single compile invocation without -main-file.
  SingleInvocation,
  /// Compile and execute the inputs immediately
  ImmediateInvocation,
};
enum class LTOKind { None, Full, Thin, Unknown };

class BuildProfile final {
 public:
  /// All of the input files that have been created
  InputFiles inputFiles;

  /// The Activitys which were used to build the Jobs.
  llvm::SmallVector<std::unique_ptr<const Activity>, 32> activities;

  /// The Jobes which will be executed by this compilation.
  llvm::SmallVector<std::unique_ptr<const Job>, 32> procs;

  /// The inputs for the linker -- may not need this there
  llvm::SmallVector<const Activity *, 2> linkerInputs;

  /// Default compile type
  CompileType compileType = CompileType::None;

  LTOKind ltoVariant = LTOKind::None;

  /// The number of threads for multi-threaded compilation.
  unsigned numThreads = 0;

  /// Returns true if multi-threading is enabled.
  bool IsMultiThreading() const { return numThreads > 0; }

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;

  // Whether or not the driver should generate a module.
  bool generateModule = false;

  /// Default linking kind
  LinkType linkType = LinkType::None;

  bool ShouldLink() { return linkType != LinkType::None; }

  /// The output type which should be used for the compiler
  file::FileType compilerOutputFileType = file::FileType::INVALID;

 public:
  class ModuleInputs final {
   private:
    llvm::SmallVector<const Activity *, 2> inputs;
  };
  class LinkerInputs final {
   private:
    llvm::SmallVector<const Activity *, 2> inputs;
  };
};

class DriverCache final {
 public:
  /// A map for caching Jobs for a given Activity/ToolChain pair
  llvm::DenseMap<std::pair<const Activity *, const ToolChain *>, Job *>
      procChacheMap;
  /// Cache of all the ToolChains in use by the driver.
  ///
  /// This maps from the string representation of a triple to a ToolChain
  /// created targeting that triple. The driver owns all the ToolChain objects
  /// stored in it, and will clean them up when torn down.
  mutable llvm::StringMap<std::unique_ptr<ToolChain>> toolChainCache;
};

class Driver final : public Session {
  DriverCache cache;
  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<Compilation> compilation;
  BuildProfile profile;

 public:
  /// The options for the driver
  DriverOptions driverOpts;

  /// The name the driver was invoked as.
  std::string driverName;

  /// Driver title to use with help.
  std::string driverTitle;

  /// The path the driver executable was in, as invoked from the
  /// command line.
  std::string driverDir;

  /// The original path to the stone executable.
  std::string stoneExecutablePath;

  /// sysroot, if present
  std::string sysRoot;

  /// Dynamic loader prefix, if present
  std::string dyldPrefix;

  /// Information about the host which can be overridden by the user.
  std::string hostBits, hostMachine, hostSystem, hostRelease;

  /// Target and driver mode components extracted from clang executable name.
  // ParsedClangName ClangNameParts;

  /// The path to the installed stone directory, if any.
  std::string installedDir;

  /// The path to the compiler resource directory.
  std::string resourceDir;

  /// System directory for config files.
  std::string systemConfigDir;

  /// User directory for config files.
  std::string userConfigDir;

 private:
  /// Name of configuration file if used.
  std::string cfgFile;

  /// Whether to check that input files exist when constructing compilation
  /// processes.
  unsigned checkInputFilesExist : 1;

  /// Arguments originated from configuration file.
  std::unique_ptr<llvm::opt::InputArgList> cfgOpts;

 private:
  void BuildActivities();
  void BuildJobs();
  void BuildQueue();

  /// This uses a std::unique_ptr instead of returning a toolchain by value
  /// because ToolChain has virtual methods.
  std::unique_ptr<ToolChain> BuildToolChain(
      const llvm::opt::InputArgList &argList);

  void BuildInputs(const ToolChain &tc, const DerivedArgList &args,
                   InputFiles &inputs);

  // std::unique_ptr<DriverInputInstance>
  // BuildInputInstance(const ToolChain &tc, const DerivedArgList &args,
  //                   InputFiles &inputs);

  /// Construct the OutputInfo for the driver from the given arguments.
  ///
  /// \param TC The current tool chain.
  /// \param Args The input arguments.
  /// \param BatchMode Whether the driver has been explicitly or implicitly
  /// instructed to use batch mode.
  /// \param Inputs The inputs to the driver.
  /// \param[out] OI The OutputInfo in which to store the resulting output
  /// information.

  void BuildOutputs(const ToolChain &toolChain,
                    const llvm::opt::DerivedArgList &args, const bool batchMode,
                    const InputFiles &inputs /*TODO: DriverInputs*/,
                    BuildProfile &profile) const;

  std::unique_ptr<Compilation> BuildCompilation(
      const ToolChain &toolChain, const llvm::opt::InputArgList &argList);

  bool HandleImmediateArgs(const ArgList &args, const ToolChain &tc);

 public:
  Driver(llvm::StringRef executablePath, std::string driverName);

  /// Parse the given list of strings into an InputArgList.
  bool Build(llvm::ArrayRef<const char *> args) override;
  int Run() override;
  void PrintLifecycle() override;
  void PrintHelp(bool showHidden) override;

 public:
  const std::string &GetConfigFile() const { return cfgFile; }

  bool GetCheckInputFilesExist() const { return checkInputFilesExist; }
  void SetCheckInputFilesExist(bool v) { checkInputFilesExist = v; }

  const std::string &GetDriverTitle() { return driverTitle; }
  void SetDriverTitle(std::string v) { driverTitle = std::move(v); }

  /// Get the path to where the clang executable was installed.
  const char *GetInstalledDir() const {
    if (!installedDir.empty()) return installedDir.c_str();
    return driverDir.c_str();
  }
  void SetInstalledDir(llvm::StringRef v) { installedDir = std::string(v); }

  const ToolChain &GetToolChain() const { return *toolChain.get(); }
  ToolChain &GetToolChain() { return *toolChain.get(); }

  const BuildProfile &GetProfile() const { return profile; }
  BuildProfile &GetProfile() { return profile; }

  const DriverCache &GetCache() const { return cache; }
  DriverCache &GetCache() { return cache; }

 protected:
  void ComputeMode(const llvm::opt::DerivedArgList &args) override;
  ModeKind GetDefaultModeKind() override;

  /// TranslateInputArgs - Create a new derived argument list from the input
  /// arguments, after applying the standard argument translations.
  // llvm::opt::DerivedArgList *
  // TranslateInputArgs(const llvm::opt::InputArgList &args) override const;
 private:
  // Build Activitys
  void BuildCompileActivities(Compilation &compilation,
                              CompilationActivity *le = nullptr);
  void BuildCompileActivity(Compilation &compilation, InputActivity *ie,
                            CompilationActivity *le = nullptr);

  void BuildJobsForCompileActivity(Compilation &compilation,
                                   const CompileActivity *ce);

  //
  void BuildLinkActivity();
  void BuildStaticLinkActivity();
  void BuildStaticLinkActivity(
      CompilationActivity &event);  // Calls BuildProcForActivity

  void BuildDynamicLinkActivity();
  void BuildDynamicLinkActivity(CompilationActivity &event);

  void BuildBackendActivity();
  void BuildAssemblyActivity();
};
}  // namespace driver
}  // namespace stone
#endif
