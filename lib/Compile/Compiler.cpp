#include "stone/Compile/Compiler.h"

#include "stone/Compile/Analysis.h"
#include "stone/Compile/Frontend.h"
#include "stone/Core/Ret.h"

using namespace stone;
using namespace stone::opts;
using namespace stone::analysis;

Compiler::Compiler(CompilePipeline *pipeline)
    : Session(compileOpts),
      pipeline(pipeline),
      fm(compileOpts.fsOpts),
      sm(GetDiagEngine(), fm) {
  analysis.reset(new Analysis(*this, compileOpts, GetSrcMgr()));
}

void Compiler::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
}

bool Compiler::Build(llvm::ArrayRef<const char *> args) {
  excludedFlagsBitmask = opts::NoCompileOption;
  auto argList = BuildArgList(args);

  std::unique_ptr<llvm::opt::DerivedArgList> dArgList(
      TranslateInputArgs(*argList));
  // Computer the compiler mode.
  ComputeMode(*dArgList);

  BuildInputs();

  // Setup the main module
  // if (!mainModule) {
  //  Identifier moduleName =
  //  astCtx->GetIdentifier(GetInvocation().GetModuleName()); mainModule =
  //  Module::Create(moduleName, *Context);
  //}

  return true;
}
void Compiler::BuildInputs() {}
ModeKind Compiler::GetDefaultModeKind() { return ModeKind::EmitObject; }

void Compiler::PrintLifecycle() {}

void Compiler::PrintHelp(bool showHidden) {}

int Compiler::Run() {
  // Perform a quick help check
  if (compileOpts.showHelp) {
    // PrintHelp();
  }
  if (GetMode().GetKind() == ModeKind::Parse) {
    Parse();
  } else {
    Check();
  }
  return 0;
}

void Compiler::Parse() { Parse(false); }

void Compiler::Parse(bool check) {
  for (auto input : inputs) {
    // stone::analysis::Parse
    if (check) {
      if (!compileOpts.analysisOpts.wholeModuleCheck) {
        CheckSourceUnit();
      }
    }
  }
  if (check && compileOpts.analysisOpts.wholeModuleCheck) {
    CheckModule();
  }
}
void Compiler::Check() { Parse(true); }

void Compiler::CheckSourceUnit() {}

void Compiler::CheckModule() {}
