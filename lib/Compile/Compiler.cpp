#include "stone/Compile/Compiler.h"

#include <algorithm>
#include <memory>

#include "stone/Core/Ret.h"

using namespace stone;
using namespace stone::opts;
using namespace stone::syntax;

Compiler::Compiler(CompilePipeline *pipeline)
    : Session(compileOpts),
      pipeline(pipeline),
      fm(compileOpts.fsOpts),
      sm(GetDiagEngine(), fm) {
  ac.reset(new ASTContext(*this, compileOpts.spOpts, sm));
}

void Compiler::ComputeMode(const llvm::opt::DerivedArgList &args) {
  Session::ComputeMode(args);
}

Module *Compiler::GetMainModule() const {
  assert(mainModule && "Null Main Module.");
  return mainModule;
}

void Compiler::SetMainModule(Module *m) {}

bool Compiler::Build(llvm::ArrayRef<const char *> args) {
  excludedFlagsBitmask = opts::NoCompileOption;

  auto argList = BuildArgList(args);

  std::unique_ptr<llvm::opt::DerivedArgList> dArgList(
      TranslateInputArgs(*argList));
  // Computer the compiler mode.
  ComputeMode(*dArgList);

  BuildInputs();

  // Setup the main module
  if (!mainModule) {
    // Identifier moduleName = ac->GetIdentifier(GetModuleName());
    // mainModule = Module::Create(moduleName, *Context);
  }

  return true;
}
void Compiler::BuildInputs() {}

ModeKind Compiler::GetDefaultModeKind() { return ModeKind::EmitObject; }

void Compiler::PrintLifecycle() {}

void Compiler::PrintHelp(bool showHidden) {}
