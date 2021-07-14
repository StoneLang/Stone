#include "stone/Compile/Compiler.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Module.h"

#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <memory>
#include <sys/stat.h>
#include <system_error>
#include <time.h>
#include <utility>

using namespace stone;
using namespace stone::opts;
using namespace stone::syn;

Compiler::Compiler(PipelineEngine *pe)
    : Session(compilerOpts), pe(pe), sm(GetDiagEngine(), GetFileMgr()),
      cc(*this) {

  tc.reset(new TreeContext(*this, compilerOpts.spOpts, sm));
  syntax.reset(new Syntax(*tc.get()));

  stats.reset(new CompilerStats(*this, *this));
  GetStatEngine().Register(stats.get());
}

void Compiler::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
}

syn::Module *Compiler::GetMainModule() const {

  if (mainModule) {
    return mainModule;
  }
  Identifier &moduleName = tc->GetIdentifier(GetModuleName());
  mainModule = syntax->CreateModuleDecl(moduleName, true);
  return mainModule;
}

// llvm::vfs::FileSystem &Compiler::GetVFS() const {
//   return GetFileMgr().getVirtualFileSystem();
// }

void Compiler::SetMainModule(syn::Module *m) { mainModule = m; }

void Compiler::Init() {}

bool Compiler::Build(llvm::ArrayRef<const char *> args) {

  excludedFlagsBitmask = opts::NoCompilerOption;
  originalArgs = ParseArgList(args);

  if (HasError())
    return false;

  translatedArgs = TranslateArgList(*originalArgs);

  // Compute the compiler mode.
  ComputeMode(*translatedArgs);

  ComputeWorkingDir();

  if (HasError())
    return false;

  BuildInputs(*translatedArgs, GetInputFiles());

  if (HasError())
    return false;

  // CreateTimer();

  return true;
}

ModeType Compiler::GetDefaultModeType() { return ModeType::EmitObject; }

void Compiler::PrintLifecycle() {}

void Compiler::PrintHelp(bool showHidden) {}

void Compiler::BuildOptions() {}

int Compiler::Run() {
  // Perform a quick help check
  if (compilerOpts.printHelp) {
    PrintHelp(compilerOpts.printHelpHidden);
  } else if (compilerOpts.printVersion) {
    PrintVersion();
  } else {
    return Compiler::Run(*this);
  }
  return ret::ok;
}

// std::unique_ptr<raw_pwrite_stream>
// CompilerInstance::CreateDefaultOutputFile(bool Binary, StringRef InFile,
//                                           StringRef Extension) {
//   return CreateOutputFile(getFrontendOpts().OutputFile, Binary,
//                           /*RemoveFileOnSignal=*/true, InFile, Extension,
//                           /*UseTemporary=*/true);
// }

// std::unique_ptr<raw_pwrite_stream> Compiler::CreateNullOutputFile() {
//   return llvm::make_unique<llvm::raw_null_ostream>();
// }

std::unique_ptr<raw_pwrite_stream>
Compiler::CreateOutputFile(llvm::StringRef OutputPath, bool Binary,
                           bool RemoveFileOnSignal, StringRef InFile,
                           StringRef Extension, bool UseTemporary,
                           bool CreateMissingDirectories) {

  std::string OutputPathName;
  std::string TempPathName;
  std::error_code EC;

  std::unique_ptr<raw_pwrite_stream> OS = CreateOutputFile(
      OutputPath, EC, Binary, RemoveFileOnSignal, InFile, Extension,
      UseTemporary, CreateMissingDirectories, &OutputPathName, &TempPathName);

  if (!OS) {
    Error(0);
    // getDiagnostics().Report(diag::err_fe_unable_to_open_output) << OutputPath
    //                                                            <<
    //                                                            EC.message();
    return nullptr;
  }

  // Add the output file -- but don't try to remove "-", since this means we are
  // using stdin.
  ////TODO: AddOutputFile(
  //    OutputFile((OutputPathName != "-") ? OutputPathName : "",
  //    TempPathName));

  return OS;
}

std::unique_ptr<llvm::raw_pwrite_stream> Compiler::CreateOutputFile(
    llvm::StringRef OutputPath, std::error_code &Error, bool Binary,
    bool RemoveFileOnSignal, StringRef InFile, StringRef Extension,
    bool UseTemporary, bool CreateMissingDirectories,
    std::string *ResultPathName, std::string *TempPathName) {

  assert((!CreateMissingDirectories || UseTemporary) &&
         "CreateMissingDirectories is only allowed when using temporary files");

  std::string OutFile;
  std::string TempFile;

  if (!OutputPath.empty()) {
    OutFile = OutputPath;
  } else if (InFile == "-") {
    OutFile = "-";
  } else if (!Extension.empty()) {
    SmallString<128> Path(InFile);
    llvm::sys::path::replace_extension(Path, Extension);
    OutFile = Path.str();
  } else {
    OutFile = "-";
  }

  std::unique_ptr<llvm::raw_fd_ostream> OS;
  std::string OSFile;

  if (UseTemporary) {
    if (OutFile == "-")
      UseTemporary = false;
    else {
      llvm::sys::fs::file_status Status;
      llvm::sys::fs::status(OutputPath, Status);
      if (llvm::sys::fs::exists(Status)) {
        // Fail early if we can't write to the final destination.
        if (!llvm::sys::fs::can_write(OutputPath)) {
          Error = make_error_code(llvm::errc::operation_not_permitted);
          return nullptr;
        }

        // Don't use a temporary if the output is a special file. This handles
        // things like '-o /dev/null'
        if (!llvm::sys::fs::is_regular_file(Status))
          UseTemporary = false;
      }
    }
  }

  if (UseTemporary) {
    // Create a temporary file.
    // Insert -%%%%%%%% before the extension (if any), and because some tools
    // (noticeable, clang's own GlobalModuleIndex.cpp) glob for build
    // artifacts, also append .tmp.
    StringRef OutputExtension = llvm::sys::path::extension(OutFile);
    SmallString<128> TempPath =
        StringRef(OutFile).drop_back(OutputExtension.size());
    TempPath += "-%%%%%%%%";
    TempPath += OutputExtension;
    TempPath += ".tmp";
    int fd;
    std::error_code EC =
        llvm::sys::fs::createUniqueFile(TempPath, fd, TempPath);

    if (CreateMissingDirectories &&
        EC == llvm::errc::no_such_file_or_directory) {
      StringRef Parent = llvm::sys::path::parent_path(OutputPath);
      EC = llvm::sys::fs::create_directories(Parent);
      if (!EC) {
        EC = llvm::sys::fs::createUniqueFile(TempPath, fd, TempPath);
      }
    }

    if (!EC) {
      OS.reset(new llvm::raw_fd_ostream(fd, /*shouldClose=*/true));
      OSFile = TempFile = TempPath.str();
    }
    // If we failed to create the temporary, fallback to writing to the file
    // directly. This handles the corner case where we cannot write to the
    // directory, but can write to the file.
  }

  if (!OS) {
    OSFile = OutFile;
    OS.reset(new llvm::raw_fd_ostream(
        OSFile, Error,
        (Binary ? llvm::sys::fs::F_None : llvm::sys::fs::F_Text)));
    if (Error)
      return nullptr;
  }

  // Make sure the out stream file gets removed if we crash.
  if (RemoveFileOnSignal)
    llvm::sys::RemoveFileOnSignal(OSFile);

  if (ResultPathName)
    *ResultPathName = OutFile;
  if (TempPathName)
    *TempPathName = TempFile;

  if (!Binary || OS->supportsSeeking())
    return std::move(OS);

  auto B = llvm::make_unique<llvm::buffer_ostream>(*OS);
  assert(!nonSeekStream);
  nonSeekStream = std::move(OS);
  return std::move(B);
}

void CompilerStats::Print() {

  // if print-stats
  // GetTimer().stopTimer();
  // auto timeRecord = GetTimer().getTotalTime();
  // timeRecord.print(timeRecord, compiler.Out().GetOS());
}

CompilerContext::~CompilerContext() {}
