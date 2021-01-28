#include "stone/Driver/Driver.h"

#include "llvm/Config/llvm-config.h"
#include "llvm/Support/FormatVariadic.h"
#include "stone/Core/Ret.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Session/ModeKind.h"

using namespace stone;
using namespace stone::file;
using namespace stone::driver;

using namespace llvm::opt;

class DriverInternal final {
 public:
  /// Pointers to the compile jobs created by Compilation -- Compilation manages
  /// these jobs
  Jobs compileJobs;

  /// Pointers to the jobs created by Compilation -- Compilation manages these
  /// jobs.
  Jobs linkerJobs;

  /// Pointers to the jobs created by Compilation -- Compilation manages these
  /// jobs.
  Jobs topLevelJobs;

 public:
  void AddCompileJob(const Job *job) { compileJobs.push_back(job); }
  void AddLinkerJob(const Job *job) { linkerJobs.push_back(job); }
  void AddTopLevelJob(const Job *job) { topLevelJobs.push_back(job); }

 public:
  static bool DoesInputExist(Driver &driver, const DerivedArgList &args,
                             llvm::StringRef input);

 public:
  /// Print the job
  static void PrintJob(const Job *job, const Driver &driver);

  /// Build compile only jobs
  static void BuildCompileOnlyJobs(Driver &driver, DriverInternal &internal);

  /// Build jobs for multiple compiles -- each job gets one source file
  static void BuildJobsForMultipleCompile(Driver &driver,
                                          DriverInternal &internal);

  /// Build jobs for a single compile -- the compile jobs has multiple files.
  static void BuildJobsForSingleCompile(Driver &driver,
                                        DriverInternal &internal);
};

void DriverInternal::BuildCompileOnlyJobs(Driver &driver,
                                          DriverInternal &internal) {}

void DriverInternal::BuildJobsForMultipleCompile(Driver &driver,
                                                 DriverInternal &internal) {
  for (const auto &input : driver.GetDriverOptions().inputs) {
    switch (input.first) {
      case FileType::Stone: {
        assert(file::IsPartOfCompilation(input.first));
        auto job = driver.GetCompilation().CreateJob<CompileJob>(
            true, driver.GetCompilation());
        job->AddInput(input);
        internal.AddCompileJob(job);
        break;
      }
      case FileType::Object:
        break;
      default:
        break;
    }
  }
}
void DriverInternal::BuildJobsForSingleCompile(Driver &driver,
                                               DriverInternal &internal) {
  auto job = driver.GetCompilation().CreateJob<CompileJob>(
      true, driver.GetCompilation());

  for (const auto &input : driver.GetDriverOptions().inputs) {
    switch (input.first) {
      case file::FileType::Stone: {
        assert(file::IsPartOfCompilation(input.first));
        job->AddInput(input);
        break;
      }
      default:
        break;
    }
  }
}
/// Check that the file referenced by \p Input exists. If it doesn't,
/// issue a diagnostic and return false.
bool DriverInternal::DoesInputExist(Driver &driver, const DerivedArgList &args,
                                    llvm::StringRef input) {
  // TODO:
  // if (!driver.GetCheckInputFilesExist())
  //  return true;

  // stdin always exists.
  if (input == "-") return true;

  if (file::Exists(input)) return true;

  driver.Out() << "de.D(SourceLoc(),"
               << "diag::error_no_such_file_or_directory, Input);" << input
               << '\n';

  return false;
}
void DriverInternal::PrintJob(const Job *job, const Driver &driver) {
  std::string str;
  // driver.Out() << job->GetName() << "," << '\n';
  if (job->GetType() == JobType::Compile) {
    const auto *cj = dyn_cast<CompileJob>(job);
    // driver.Out() << "\"" << input->GetInput().second << "\"";
  }
}

Driver::Driver(llvm::StringRef stoneExecutable, std::string driverName)
    : Session(driverOpts),
      stoneExecutablePath(stoneExecutablePath),
      driverName(driverName),
      /*sysRoot(DEFAULT_SYSROOT),*/
      driverTitle("Stone Compiler"),
      checkInputFilesExist(true) {
  stats.reset(new DriverStats(*this));
  GetStatEngine().Register(stats.get());
}
/// Parse the given list of strings into an InputArgList.
bool Driver::Build(llvm::ArrayRef<const char *> args) {
  excludedFlagsBitmask = opts::NoDriverOption;

  originalArgs = BuildArgList(args);
  // TODO: Check for errors

  BuildToolChain(*originalArgs);
  // TODO: Check for errors

  BuildCompilation(*toolChain, *originalArgs);

  if (de.HasError()) {
    return false;
  }

  return true;
}

void Driver::BuildToolChain(const llvm::opt::InputArgList &argList) {
  if (const llvm::opt::Arg *arg = argList.getLastArg(opts::Target)) {
    targetTriple = llvm::Triple::normalize(arg->getValue());
  }
  llvm::Triple target(targetTriple);

  switch (target.getOS()) {
    case llvm::Triple::Darwin:
    case llvm::Triple::MacOSX: {
      llvm::Optional<llvm::Triple> targetVariant;
      if (const llvm::opt::Arg *A = argList.getLastArg(opts::TargetVariant)) {
        targetVariant = llvm::Triple(llvm::Triple::normalize(A->getValue()));
      }
      toolChain =
          llvm::make_unique<DarwinToolChain>(*this, target, targetVariant);
      break;
    }
      /*
          case llvm::Triple::Linux:
            toolChain = llvm::make_unique<stone::Linux>(*this, target);
            break;
          case llvm::Triple::FreeBSD:
            toolChain = llvm::make_unique<stone::FreeBSD>(*this, target);
            break;
          case llvm::Triple::OpenBSD:
            toolChain = llvm::make_unique<stone::OpenBSD>(*this, target);
            break;
          case llvm::Triple::Win32:
            toolChain = llvm::make_unique<stone::Win>(*this, target);
            break;
      */
    default:
      Out() << "D(SourceLoc(),"
            << "msg::error_unknown_target,"
            << "ArgList.getLastArg(opts::Target)->getValue());" << '\n';
      break;
  }
}
void Driver::BuildCompilation(const ToolChain &tc,
                              const llvm::opt::InputArgList &argList) {
  llvm::PrettyStackTraceString CrashInfo("Compilation construction");

  // TODO:
  // workingDir = ComputeWorkingDir(argList.get());

  // NOTE: Session manages this object
  auto dArgList = TranslateInputArgs(argList);

  // Computer the compiler mode.
  ComputeMode(*dArgList);

  // Perform toolchain specific args validation.
  // toolChain.ValidateArguments(de, *dArgList, targetTriple);
  //
  if (de.HasError()) {
    return;
  }
  if (CutOff(*dArgList, tc)) {
    return;
  }

  BuildInputs(tc, *dArgList, GetInputs());

  if (GetInputs().size() == 0) {
    Out() << "msg::driver_error_no_input_files" << '\n';
    return;
  }

  if (de.HasError()) return;

  /*
  BuildOutputs(GetToolChain(),
                          const llvm::opt::DerivedArgList &args,
                          const bool batchMode,
                                                                                                                          const InputFiles &inputs,
                                                                                                                          DriverRuntime& runtime)

  */

  // TODO: ComputeCompileMod()
  //
  // About to move argument list, so capture some flags that will be needed
  // later.
  driverOpts.printJobs = dArgList->hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle = dArgList->hasArg(opts::PrintDriverLifecycle);
  driverOpts.printStats = dArgList->hasArg(opts::PrintDriverStats);

  compilation = llvm::make_unique<Compilation>(*this);

  BuildJobs();
}
bool Driver::CutOff(const ArgList &args, const ToolChain &tc) {
  if (args.hasArg(opts::Help)) {
    PrintHelp(false);
    return true;
  } else if (args.hasArg(opts::Version)) {
    PrintVersion();
    return true;
  }
  return false;
}

// TODO: May move to session
void Driver::BuildInputs(const ToolChain &tc, const DerivedArgList &args,
                         InputFiles &inputs) {
  llvm::DenseMap<llvm::StringRef, llvm::StringRef> seenSourceFiles;

  for (Arg *arg : args) {
    if (arg->getOption().getKind() == Option::InputClass) {
      llvm::StringRef argValue = arg->getValue();
      file::FileType ft = file::FileType::INVALID;
      // stdin must be handled specially.
      if (argValue.equals("-")) {
        // By default, treat stdin as Swift input.
        ft = file::FileType::Stone;
      } else {
        // Otherwise lookup by extension.
        ft = file::GetTypeByExt(file::GetExt(argValue));

        if (ft == file::FileType::INVALID) {
          // By default, treat inputs with no extension, or with an
          // extension that isn't recognized, as object files.
          ft = file::FileType::Object;
        }
      }

      if (DriverInternal::DoesInputExist(*this, args, argValue)) {
        driverOpts.AddInput(ft, argValue);
      }

      if (ft == FileType::Stone) {
        auto basename = llvm::sys::path::filename(argValue);
        if (!seenSourceFiles.insert({basename, argValue}).second) {
          Out() << "de.D(SourceLoc(),"
                << "diag::error_two_files_same_name,"
                << "basename, seenSourceFiles[basename], argValue);" << '\n';
          Out() << " de.D(SourceLoc(), "
                << "diag::note_explain_two_files_"
                   "same_name);"
                << '\n';
        }
      }
    }
  }
}

void Driver::BuildOutputs(const ToolChain &toolChain,
                          const llvm::opt::DerivedArgList &args,
                          const bool batchMode, const InputFiles &inputs,
                          DriverRuntime &runtime) const {
  // Basic for the time being
  switch (mode.GetKind()) {
    case ModeKind::EmitExecutable:
      runtime.linkType = LinkType::Executable;
      break;
    default:
      break;
  }
}

ModeKind Driver::GetDefaultModeKind() { return ModeKind::EmitExecutable; }

void Driver::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
}

void Driver::BuildJobs() {
  llvm::PrettyStackTraceString CrashInfo("Building compilation jobs.");

  if (GetDriverOptions().inputs.empty()) {
    Out() << "D(SrcLoc(), msg::error_no_input_files)" << '\n';
    return;
  }

  // TODO: BuildCompileOnlyJobs();
  DriverInternal driverInternal;
  switch (runtime.compilerInvocationMode) {
    case CompilerInvocationMode::Multiple:
      DriverInternal::BuildJobsForMultipleCompile(*this, driverInternal);
      break;
    case CompilerInvocationMode::Single:
      DriverInternal::BuildJobsForSingleCompile(*this, driverInternal);
      break;
    default:
      break;
  }

  /*
    if (runtime.RequiresLink() && !driverInternal.compileJobs.empty()) {
      Job *linkJob = nullptr;
      switch (runtime.linkType) {
        case LinkType::StaticLibrary: {
          linkJob = GetCompilation().CreateJob<StaticLinkJob>(
              true, GetCompilation(), runtime.RequiresLTO(),
              runtime.linkType);
        }
        case LinkType::DynamicLibrary: {
          linkJob = GetCompilation().CreateJob<DynamicLinkJob>(
              true, GetCompilation(), runtime.RequiresLTO(),
              runtime.linkType);
        }
        default:
          break;
      }
      assert(linkJob && "LinkJob was not created -- requires linking.");
      for (auto job : driverInternal.compileJobs) {
        linkJob->AddDep(job);
      }
    }
  */
}

void Driver::PrintJobs() {
  for (const auto &job : GetCompilation().GetJobs()) {
    DriverInternal::PrintJob(&job, *this);
  }
}

/*
void Driver::BuildActivities() {
  llvm::PrettyStackTraceString CrashInfo("Building compilation activities");

  if (mode.IsCompileOnly()) {
    BuildCompileActivities();
  } else {
    BuildLinkActivity();
  }
}
void Driver::BuildCompileActivities() {
  for (const auto &input : GetDriverOptions().inputs) {
    auto inputActivity = GetCompilation().CreateActivity<InputActivity>(input);
    switch (input.first) {
      case file::FileType::Stone: {
        assert(file::IsPartOfCompilation(input.first));
        BuildCompileActivity(inputActivity);
        break;
      }
      default:
        break;
    }
  }
}
void Driver::BuildCompileActivity(InputActivity *inputActivity) {
  if (runtime.compilerInvocationMode == CompilerInvocationMode::Multiple) {
    auto compileActivity = GetCompilation().CreateActivity<CompileActivity>(
        inputActivity, runtime.compilerOutputFileType);

    runtime.AddModuleInput(compileActivity);
    if (runtime.ShouldLink()) {
      runtime.AddLinkerInput(compileActivity);
    }
    // BuildJobsForActivity(compileActivity);

  } else if (runtime.compilerInvocationMode == CompilerInvocationMode::Single) {
    // TODO:
  }

  // TODO: May just want build BuildJobsForActivity
  //
  // Since we are here, let us build the jobs.
}

void Driver::BuildJobsForActivity(const CompilationActivity *activity) {}

void Driver::BuildLinkActivity() {
  // First, build all the compile activities
  BuildCompileActivities();

  if (runtime.ShouldLink() && !runtime.linkerActivities.empty()) {
    Activity *linkActivity = nullptr;
    if (runtime.linkType == LinkType::StaticLibrary) {
      linkActivity = GetCompilation().CreateActivity<StaticLinkActivity>(
          runtime.linkerActivities, runtime.linkType);
    } else {
      linkActivity = GetCompilation().CreateActivity<DynamicLinkActivity>(
          runtime.linkerActivities, runtime.linkType,
          runtime.ShouldPerformLTO());
    }
    runtime.AddTopLevelActivity(linkActivity);
  }
  // BuildJobForActivity()
}

*/

void Driver::PrintLifecycle() {}

void Driver::PrintHelp(bool showHidden) {
  excludedFlagsBitmask = opts::NoDriverOption;
  // if (!showHidden)
  //  excludedFlagsBitmask |= HelpHidden;

  driverOpts.GetOpts().PrintHelp(Out(), driverName.c_str(), "Stone Compiler",
                                 includedFlagsBitmask, excludedFlagsBitmask,
                                 /*ShowAllAliases*/ false);
}

void Driver::ComputeModuleOutputPath() {}

void Driver::ComputeMainOutput() {}

int Driver::Run() {
  // Perform a quick help check
  if (driverOpts.printHelp) {
    // PrintHelp();
  }
  return GetCompilation().Run();
}
