#include "stone/Compile/Compiler.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Module.h"

#include <algorithm>
#include <memory>

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
  assert(mainModule && "Null Main Module.");

  // Setup the main module
  if (!mainModule) {
    Identifier &moduleName = tc->GetIdentifier(GetModuleName());
    mainModule = syn::Module::CreateMainModule(moduleName, *tc);
  }
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
    return Compiler::Compile(*this);
  }
  return ret::ok;
}

void CompilerStats::Print() {

  // if print-stats
  // GetTimer().stopTimer();
  // auto timeRecord = GetTimer().getTotalTime();
  // timeRecord.print(timeRecord, compiler.Out().GetOS());
}

CompilerContext::~CompilerContext() {}
