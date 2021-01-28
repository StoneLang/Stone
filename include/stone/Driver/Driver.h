#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "stone/Core/Mem.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/DriverStats.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/JobOptions.h"
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
class Compilation;
class ToolChain;
class DriverStats;

enum class CompilerInvocationMode {
  None,
  /// Multiple compile invocations and -main-file.
  Multiple,
  /// A single compilation using a single compile invocation without -main-file.
  Single,
};
enum class LTOKind { None, Full, Thin, Unknown };

class DriverCache final {
 public:
  /// A map for caching Jobs for a given ToolChain pair
  llvm::DenseMap<const ToolChain *, Job *> jobCache;
  /// Cache of all the ToolChains in use by the driver.
  ///
  /// This maps from the string representation of a triple to a ToolChain
  /// created targeting that triple. The driver owns all the ToolChain objects
  /// stored in it, and will clean them up when torn down.
  mutable llvm::StringMap<std::unique_ptr<ToolChain>> toolChainCache;
};

class DriverRuntime final {
 public:
  /// Default compiler invocation mode -- one file per CompileJob
  CompilerInvocationMode compilerInvocationMode =
      CompilerInvocationMode::Multiple;

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

  /// The output type which should be used for the compiler
  file::FileType compilerOutputFileType = file::FileType::INVALID;

  /// Whether or not the driver should generate a module.
  bool shouldGenerateModule = false;

  DriverCache cache;

  const DriverCache &GetCache() const { return cache; }
  DriverCache &GetCache() { return cache; }

  bool RequiresLTO() { return ltoVariant != LTOKind::None; }
  bool RequiresLink() { return linkType != LinkType::None; }
};

class Driver final : public Session {
  DriverRuntime runtime;
  std::unique_ptr<DriverStats> stats;
  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<Compilation> compilation;
  friend DriverStats;

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
  // void BuildActivities();
  /// This uses a std::unique_ptr instead of returning a toolchain by value
  /// because ToolChain has virtual methods.
  void BuildToolChain(const llvm::opt::InputArgList &args);

  void BuildInputs(const ToolChain &tc, const DerivedArgList &args,
                   InputFiles &inputFiles);

  /// TODO: A CompilationUnit consists of an InputFile and an OutputFile
  void BuildCompilationUnits(const ToolChain &tc, const DerivedArgList &args);

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
                    const InputFiles &inputs, DriverRuntime &runtime) const;

  void BuildCompilation(const ToolChain &tc,
                        const llvm::opt::InputArgList &args);

  bool CutOff(const ArgList &args, const ToolChain &tc);

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

  const DriverRuntime &GetRuntime() const { return runtime; }
  DriverRuntime &GetRuntime() { return runtime; }

  Compilation &GetCompilation() { return *compilation.get(); }

  DriverOptions &GetDriverOptions() { return driverOpts; }

  DriverStats &GetStats() { return *stats.get(); }

  void ComputeModuleOutputPath();
  void ComputeMainOutput();

 protected:
  void ComputeMode(const llvm::opt::DerivedArgList &args) override;
  ModeKind GetDefaultModeKind() override;

  /// TranslateInputArgs - Create a new derived argument list from the input
  /// arguments, after applying the standard argument translations.
  // llvm::opt::DerivedArgList *
  // TranslateInputArgs(const llvm::opt::InputArgList &args) override;
 private:
  static llvm::StringRef GetOutputFileName();

 private:
  void BuildJobs();
  void PrintJobs();
  void BuildJobQueue();
};
}  // namespace driver
}  // namespace stone
#endif
