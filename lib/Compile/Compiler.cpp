#include "stone/Compile/Compiler.h"
#include "stone/Utils/Ret.h"

#include <algorithm>
#include <memory>

using namespace stone;
using namespace stone::opts;
using namespace stone::syn;

Compiler::Compiler(Pipeline *pipeline)
    : Session(compilerOpts), pipeline(pipeline), fm(compilerOpts.fsOpts),
      sm(GetDiagEngine(), fm) {
  tc.reset(new TreeContext(*this, compilerOpts.spOpts, sm));

  stats.reset(new CompilerStats(*this));
  GetStatEngine().Register(stats.get());
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

  originalArgs = ParseArgList(args);

  translatedArgs = TranslateArgList(*originalArgs);
  // Computer the compiler mode.
  ComputeMode(*translatedArgs);

  BuildInputs();

  // Setup the main module
  if (!mainModule) {
    // Identifier moduleName = ac->GetIdentifier(GetModuleName());
    // mainModule = Module::Create(moduleName, *Context);
  }

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
    return Compiler::Run(*this);
  }
  return ret::ok;
}

/* TODO:
std::unique_ptr<Compiler> Compiler::Create() {
        std::unique_ptr<Compiler> compiler (new Compiler()):

}

*/

void CompilerStats::Print() {}
