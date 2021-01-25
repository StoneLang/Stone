#include "stone/Driver/Driver.h"

#include "llvm/Config/llvm-config.h"
#include "llvm/Support/FormatVariadic.h"
#include "stone/Core/Ret.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

using namespace llvm::opt;

Driver::Driver(llvm::StringRef stoneExecutable, std::string driverName)
    : Session(driverOpts),
      stoneExecutablePath(stoneExecutablePath),
      driverName(driverName),
      /*sysRoot(DEFAULT_SYSROOT),*/
      driverTitle("Stone Compiler"),
      checkInputFilesExist(true) {}
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
          return llvm::make_unique<stone::Linux>(*this, target);
        case llvm::Triple::FreeBSD:
          return llvm::make_unique<stone::FreeBSD>(*this, target);
        case llvm::Triple::OpenBSD:
          return llvm::make_unique<stone::OpenBSD>(*this, target);
        case llvm::Triple::Win32:
          return llvm::make_unique<stone::Win>(*this, target);
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

  BuildInputFiles(tc, *dArgList, GetInputFiles());

  if (GetInputFiles().size() == 0) {
    Out() << "msg::driver_error_no_input_files" << '\n';
    return;
  }

  if (de.HasError()) return;

  // TODO: ComputeCompileMod()
  //
  // About to move argument list, so capture some flags that will be needed
  // later.
  // const bool driverPrintActions =
  //    ArgList->hasArg(opts::DriverPrintActions);

  // const bool DriverPrintDerivedOutputFileMap =
  //    ArgList->hasArg(options::OPT_driver_print_derived_output_file_map);

  // const bool ContinueBuildingAfterErrors =
  //    computeContinueBuildingAfterErrors(BatchMode, ArgList.get());

  // driverOpts.showLifecycle = argList.hasArg(opts::ShowLifecycle);

  compilation = llvm::make_unique<Compilation>(*this);

  BuildActivities();
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

/// Check that the file referenced by \p Input exists. If it doesn't,
/// issue a diagnostic and return false.
static bool DoesInputExist(Driver &driver, const DerivedArgList &args,
                           DiagnosticEngine &de, llvm::StringRef input) {
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
// TODO: May move to session
void Driver::BuildInputFiles(const ToolChain &tc, const DerivedArgList &args,
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

      if (DoesInputExist(*this, args, de, argValue)) {
        inputs.push_back(std::make_pair(ft, arg));
      }

      if (ft == file::FileType::Stone) {
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

void Driver::BuildOutputFiles(const ToolChain &toolChain,
                              const llvm::opt::DerivedArgList &args,
                              const bool batchMode, const InputFiles &inputs,
                              DriverProfile &profile) const {}

ModeKind Driver::GetDefaultModeKind() { return ModeKind::EmitExecutable; }

void Driver::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
}

void Driver::BuildActivities() {
  llvm::PrettyStackTraceString CrashInfo("Building compilation activities");

  if (mode.IsCompileOnly()) {
    BuildCompileActivities(*compilation.get());
  } else {
    BuildLinkActivity(*compilation.get());
  }
}
void Driver::BuildCompileActivities(Compilation &compilation,
                                    CompilationActivity *linkActivity) {
  // Go through the files and build the compile activities

  for (const InputPair &input : inputFiles) {
    // BuildCompileActivity(input);
    file::FileType inputType = input.first;
    const llvm::opt::Arg *inputArg = input.second;
    auto inputActivity =
        compilation.CreateActivity<InputActivity>(*inputArg, inputType);

    switch (inputType) {
      case file::FileType::Stone: {
        assert(file::IsPartOfCompilation(inputType));
        BuildCompileActivity(compilation, inputActivity, linkActivity);
      }
      default:
        break;
    }
  }
}
void Driver::BuildCompileActivity(Compilation &compilation,
                                  InputActivity *inputActivity,
                                  CompilationActivity *linkActivity) {
  if (profile.compilerInvocationMode == CompilerInvocationMode::Multiple) {
    // TODO:
  } else if (profile.compilerInvocationMode == CompilerInvocationMode::Single) {
    // TODO:
  }
  auto compileActivity = compilation.CreateActivity<CompileActivity>(
      inputActivity, profile.compilerOutputFileType);

  // TODO: May just want build BuildJobsForActivity
  //
  // Since we are here, let us build the jobs.
  BuildJobsForCompileActivity(compilation, compileActivity);
}

void Driver::BuildJobsForCompileActivity(Compilation &compilation,
                                         const CompileActivity *activity) {}

void Driver::BuildLinkActivity(Compilation &compilation) {
  // BuildCompileActivities();

  /// NOTE: Defaulting to static link
  auto linkActivity = compilation.CreateActivity<StaticLinkActivity>(
      nullptr, driver::LinkType::Executable);

  BuildCompileActivities(compilation, linkActivity);
}
static void BuildJob(Driver &driver) {}
void Driver::BuildJobs() {}

void Driver::PrintLifecycle() {}

void Driver::PrintHelp(bool showHidden) {
  excludedFlagsBitmask = opts::NoDriverOption;
  // if (!showHidden)
  //  excludedFlagsBitmask |= HelpHidden;

  driverOpts.GetOpts().PrintHelp(Out(), driverName.c_str(), "Stone Compiler",
                                 includedFlagsBitmask, excludedFlagsBitmask,
                                 /*ShowAllAliases*/ false);
}

int Driver::Run() {
  // Perform a quick help check
  if (driverOpts.showHelp) {
    // PrintHelp();
  }
  return GetCompilation().Run();
}
