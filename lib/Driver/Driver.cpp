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
  toolChain = BuildToolChain(*originalArgs);
  compilation = BuildCompilation(*toolChain, *originalArgs);

  return true;
}

std::unique_ptr<ToolChain> Driver::BuildToolChain(
    const llvm::opt::InputArgList &argList) {
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
      return llvm::make_unique<DarwinToolChain>(*this, target, targetVariant);
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
      os << "D(SourceLoc(),"
         << "msg::error_unknown_target,"
         << "ArgList.getLastArg(options::OPT_target)->getValue());" << '\n';
      break;
  }
}
std::unique_ptr<Compilation> Driver::BuildCompilation(
    const ToolChain &tc, const llvm::opt::InputArgList &argList) {
  llvm::PrettyStackTraceString CrashInfo("Compilation construction");

  // TODO:
  // workingDirectory = ComputeWorkingDirectory(argList.get());

  std::unique_ptr<DerivedArgList> dArgList(TranslateInputArgs(argList));

  // Computer the compiler mode.
  ComputeMode(*dArgList);

  // Perform toolchain specific args validation.
  // toolChain.ValidateArguments(de, *dArgList, targetTriple);
  //
  if (de.HasError()) {
    return nullptr;
  }
  if (!HandleImmediateArgs(*dArgList, tc)) {
    return nullptr;
  }

  // Start here - BuildActivities(*args, inputs);

  BuildInputs(tc, *dArgList, profile.inputFiles);

  if (de.HasError()) return nullptr;

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

  driverOpts.showLifecycle = argList.hasArg(opts::ShowLifecycle);

  compilation.reset(new Compilation(*this));

  BuildActivities();
}

bool Driver::HandleImmediateArgs(const ArgList &args, const ToolChain &tc) {
  if (args.hasArg(opts::Help)) {
    PrintHelp(false);
    return false;
  }
  return true;
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

void Driver::BuildOutputs(const ToolChain &toolChain,
                          const llvm::opt::DerivedArgList &args,
                          const bool batchMode, const InputFiles &inputs,
                          BuildProfile &profile) const {}

ModeKind Driver::GetDefaultModeKind() { return ModeKind::EmitExecutable; }

void Driver::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
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
  return 0;
}
