#include "stone/Compile/Compiler.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Module.h"

#include <algorithm>
#include <memory>

using namespace stone;
using namespace stone::opts;
using namespace stone::syn;

Compiler::Compiler(PipelineEngine *pe)
    : Session(compilerOpts), pe(pe), fm(compilerOpts.fsOpts),
      sm(GetDiagEngine(), fm), cc(*this) {

  tc.reset(new TreeContext(*this, compilerOpts.spOpts, sm));

  stats.reset(new CompilerStats(*this));
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

void Compiler::SetMainModule(syn::Module *m) { mainModule = m; }

void Compiler::Init() {}

bool Compiler::Build(llvm::ArrayRef<const char *> args) {

  excludedFlagsBitmask = opts::NoCompilerOption;

  originalArgs = ParseArgList(args);

  translatedArgs = TranslateArgList(*originalArgs);
  // Computer the compiler mode.
  ComputeMode(*translatedArgs);

  CreateTimer();

  BuildInputs();

  // Create CompilingUnits
  // for (const auto &input : compiler.GetCompilerOptions().inputs) {
  //}

  return true;
}

// TODO: Belongs in session
void Compiler::BuildInputs() {}

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
