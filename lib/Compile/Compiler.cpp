#include "stone/Compile/Compiler.h"

#include <algorithm>
#include <memory>

#include "stone/Core/Ret.h"

using namespace stone;
using namespace stone::opts;
using namespace stone::syntax;

Compiler::Compiler(Pipeline *pipeline)
    : Session(compilerOpts),
      pipeline(pipeline),
      fm(compilerOpts.fsOpts),
      sm(GetDiagEngine(), fm),
      stats(*this) {
  ac.reset(new ASTContext(*this, compilerOpts.spOpts, sm));
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

/* TODO:
std::unique_ptr<Compiler> Compiler::Create() {
        std::unique_ptr<Compiler> compiler (new Compiler()):

}

*/

void CompilerStats::Print() const {}
