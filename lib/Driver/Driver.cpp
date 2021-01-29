#include "stone/Driver/Driver.h"

#include "llvm/Config/llvm-config.h"
#include "llvm/Support/FormatVariadic.h"
#include "stone/Core/LLVM.h"
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
  Job *linkJob = nullptr;

  /// Pointers to the jobs created by Compilation -- Compilation manages these
  /// jobs.
  Jobs topLevelJobs;

 public:
  void AddCompileJob(const Job *job) { compileJobs.push_back(job); }
  void SetLinkJob(Job *job) { linkJob = job; }
  void AddTopLevelJob(const Job *job) { topLevelJobs.push_back(job); }

 public:
  static bool DoesInputExist(Driver &driver, const DerivedArgList &args,
                             llvm::StringRef input);

 public:
  /// Print the job
  static void PrintJob(Job *job, Driver &driver);

  /// Print the job
  static void PrintJobVerbosely(Job *job, Driver &driver);

  /// Build jobs for multiple compiles -- each job gets one source file
  static void BuildJobsForMultipleCompileType(Driver &driver,
                                              DriverInternal &internal);

  /// Build jobs for a single compile -- the compile jobs has multiple files.
  static void BuildJobsForSingleCompileType(Driver &driver,
                                            DriverInternal &internal);

  // TODO:
  static void ComputeCompileType(const Driver &driver,
                                 const DerivedArgList &args,
                                 const InputFiles &inputs);

  static void ComputeCompilerOutputFile(const Driver &driver);

  static void ComputeCmdOutput(const Driver &driver);

  static llvm::StringRef ComputeBaseNameForImage(const Job *job,
                                                 const OutputProfile &op,
                                                 const llvm::Triple &triple,
                                                 llvm::SmallString<128> &buffer,
                                                 llvm::StringRef baseInput,
                                                 llvm::StringRef baseName);
  /*
  static llvm:::StringRef ComputeOutputFilename(Compilation &compilation,
                             const Job *job,
                             const TypeToPathMap *OutputMap,
                             llvm::StringRef workingDir,
                             bool atTopLevel,
                             llvm::StringRef BaseInput,
                             llvm::StringRef PrimaryInput,
                             llvm::SmallString<128> &buffer);
  */

  static std::unique_ptr<driver::TaskQueue> BuildTaskQueue(
      Driver &driver, DriverInternal& internal);

 public:
  /// Builds the compile jobs
  static void BuildCompileJobs(Driver &driver, DriverInternal &internal);

  /// Builds the  link job
  static void BuildLinkJob(Driver &driver, DriverInternal &internal);

  /// Build compile only jobs
  static void BuildBackendJob(Driver &driver, DriverInternal &internal);

  /// Build compile only jobs
  static void BuildAssembleJob(Driver &driver, DriverInternal &internal);
};

/// Check that the file referenced by \p Input exists. If it doesn't,
/// issue a diagnostic and return false.
bool DriverInternal::DoesInputExist(Driver &driver, const DerivedArgList &args,
                                    llvm::StringRef input) {
  if (!driver.GetCheckInputFilesExist()) {
    return true;
  }
  // stdin always exists.
  if (input == "-") {
    return true;
  }
  if (file::Exists(input)) {
    return true;
  }
  driver.Out() << "de.D(SourceLoc(),"
               << "diag::error_no_such_file_or_directory, Input);" << input
               << '\n';
  return false;
}

void DriverInternal::BuildCompileJobs(Driver &driver,
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

void DriverInternal::BuildLinkJob(Driver &driver, DriverInternal &internal) {
  BuildCompileJobs(driver, internal);
  if (driver.GetMode().IsCompileOnly()) {
    return;
  }
  if (driver.GetOutputProfile().RequiresLink() &&
      !internal.compileJobs.empty()) {
    Job *linkJob = nullptr;
    switch (driver.GetOutputProfile().linkType) {
      case LinkType::StaticLibrary: {
        // TODO: driver.GetToolChain().Createob ...
        linkJob = driver.GetCompilation().CreateJob<StaticLinkJob>(
            true, driver.GetCompilation(),
            driver.GetOutputProfile().RequiresLTO(),
            driver.GetOutputProfile().linkType);
        break;
      }
      case LinkType::DynamicLibrary: {
        linkJob = driver.GetCompilation().CreateJob<DynamicLinkJob>(
            true, driver.GetCompilation(),
            driver.GetOutputProfile().RequiresLTO(),
            driver.GetOutputProfile().linkType);
        break;
      }
      default:
        break;
    }
    assert(linkJob && "LinkJob was not created -- requires linking.");
    for (auto job : internal.compileJobs) {
      linkJob->AddDep(job);
    }
    assert(linkJob && "LinkJob was not created.");
    internal.SetLinkJob(linkJob);
  }
}

void DriverInternal::BuildAssembleJob(Driver &driver,
                                      DriverInternal &internal) {}

void DriverInternal::BuildBackendJob(Driver &driver, DriverInternal &internal) {
}

void DriverInternal::BuildJobsForMultipleCompileType(Driver &driver,
                                                     DriverInternal &internal) {
  if (driver.GetMode().IsCompileOnly()) {
    BuildCompileJobs(driver, internal);
    return;
  }
  if (driver.GetOutputProfile().RequiresLink()) {
    BuildLinkJob(driver, internal);
    return;
  }
}
void DriverInternal::BuildJobsForSingleCompileType(Driver &driver,
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
  // job->BuildCmdOutput();
}

void DriverInternal::ComputeCompileType(const Driver &driver,
                                        const DerivedArgList &args,
                                        const InputFiles &inputs) {}

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

std::unique_ptr<driver::TaskQueue> DriverInternal::BuildTaskQueue(
    Driver &driver, DriverInternal &internal) {
  // TODO:
  return llvm::make_unique<driver::UnixTaskQueue>(driver);
}

/// Parse the given list of strings into an InputArgList.
bool Driver::Build(llvm::ArrayRef<const char *> args) {
  excludedFlagsBitmask = opts::NoDriverOption;

  originalArgs = BuildArgList(args);
  // TODO: Check for errors

  BuildToolChain(*originalArgs);
  // TODO: Check for errors

  BuildCompilation(*originalArgs);

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
void Driver::BuildCompilation(const llvm::opt::InputArgList &argList) {
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
  if (EmitInfo(*dArgList, GetToolChain())) {
    return;
  }

  BuildInputs(GetToolChain(), *dArgList, GetInputs());

  if (CutOff()) return;

  if (GetInputs().size() == 0) {
    Out() << "msg::driver_error_no_input_files" << '\n';
    return;
  }

  BuildOutputProfile(*dArgList, GetOutputProfile());

  if (CutOff()) return;

  // TODO: ComputeCompileMod()
  //
  // About to move argument list, so capture some flags that will be needed
  // later.
  driverOpts.printJobs = dArgList->hasArg(opts::PrintDriverJobs);
  driverOpts.printLifecycle = dArgList->hasArg(opts::PrintDriverLifecycle);
  driverOpts.printStats = dArgList->hasArg(opts::PrintDriverStats);

  compilation = llvm::make_unique<Compilation>(*this);

  BuildJobs();

  if (CutOff()) return;

  if (driverOpts.printJobs) {
    PrintJobs();
  }
}
bool Driver::EmitInfo(const ArgList &args, const ToolChain &tc) {
  if (args.hasArg(opts::Help)) {
    PrintHelp(false);
    return true;
  }
  if (args.hasArg(opts::Version)) {
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

void Driver::BuildOutputProfile(const llvm::opt::DerivedArgList &args,
                                OutputProfile &outputProfile) const {
  auto compilerOutputType = outputProfile.ltoVariant != LTOKind::None
                                ? FileType::BC
                                : FileType::Object;
  // By default, the driver does not link its outputProfile. this will be
  // updated appropriately below if linking is required.
  //
  outputProfile.compilerOutputFileType = compilerOutputType;

  // if (const Arg *numThreads = args.getLastArg(opts::NumThreads)) {
  // if (StringRef(A->getValue()).getAsInteger(10, outputProfile.numThreads)) {
  // Diags.diagnose(SourceLoc(), diag::error_invalid_arg_value,
  //               A->getAsString(Args), A->getValue());
  //}
  //}

  // Basic for the time being
  switch (mode.GetKind()) {
    case ModeKind::EmitLibrary: {
      outputProfile.linkType = args.hasArg(opts::Static)
                                   ? LinkType::StaticLibrary
                                   : LinkType::DynamicLibrary;
      outputProfile.compilerOutputFileType = compilerOutputType;
    } break;
    case ModeKind::EmitObject:
      outputProfile.compilerOutputFileType = FileType::Object;
      break;
    case ModeKind::EmitAssembly:
      outputProfile.compilerOutputFileType = FileType::Assembly;
      break;
    case ModeKind::EmitIR:
      outputProfile.compilerOutputFileType = FileType::IR;
      break;
    case ModeKind::EmitBC:
      outputProfile.compilerOutputFileType = FileType::BC;
      break;
    case ModeKind::Parse:
    case ModeKind::Check:
      outputProfile.compilerOutputFileType = FileType::None;
      break;
    default:
      outputProfile.linkType = LinkType::Executable;
      break;
  }
  assert(outputProfile.compilerOutputFileType != FileType::INVALID);
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
  switch (outputProfile.compileType) {
    case CompileType::Multiple:
      DriverInternal::BuildJobsForMultipleCompileType(*this, driverInternal);
      break;
    case CompileType::Single:
      DriverInternal::BuildJobsForSingleCompileType(*this, driverInternal);
      break;
    default:
      break;
  }
}

void Driver::PrintJobs() {
  for (auto &job : GetCompilation().GetJobs()) {
    DriverInternal::PrintJob(&job, *this);
  }
}

void Driver::PrintLifecycle() {}

void Driver::PrintHelp(bool showHidden) {
  excludedFlagsBitmask = opts::NoDriverOption;
  // if (!showHidden)
  //  excludedFlagsBitmask |= HelpHidden;

  driverOpts.GetOpts().PrintHelp(Out().GetOS(), driverName.c_str(),
                                 "Stone Compiler", includedFlagsBitmask,
                                 excludedFlagsBitmask,
                                 /*ShowAllAliases*/ false);
}

void DriverInternal::PrintJob(Job *job, Driver &driver) {
  auto cos = driver.Out();
  cos.UseGreen();
  switch (job->GetType()) {
    case JobType::Compile: {
      auto *cj = dyn_cast<CompileJob>(job);
      cos << job->GetName() << '\n';
      cos.UseMagenta();
      for (auto input : job->GetJobOptions().inputs) {
        cos << ' ' << input.second << ' ' << "->" << ' '
            << job->GetOutputTypeName() << '\n';
      }
      break;
    }
    default:
      break;
  }
}

void Driver::ComputeModuleOutputPath() {}

void Driver::ComputeMainOutput() {}

int Driver::Run() {
  // Perform a quick help check
  if (driverOpts.printHelp) {
    // PrintHelp();
  }
  //auto compilationResult = 
	//	GetCompilation().Run(DriverInternal::BuildTaskQueue(*this)); 

	if(CutOff()) 
		return ret::err; 

	return ret::ok; 

}
