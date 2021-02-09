#include "stone/Compile/Compile.h"
#include "stone/Frontend/Check.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerUnit.h"
#include "stone/Gen/Gen.h"
#include "stone/Frontend/Parse.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Utils/Defer.h"
#include "stone/Utils/Ret.h"

using namespace stone;

class Compiler::Implementation final {
  Compiler &compiler;
  llvm::Module *llvmModule = nullptr;

  CompilerUnit *currentUnit = nullptr;
  std::vector<CompilerUnit *> units;

public:
  Implementation(Compiler &compiler);
  ~Implementation();

public:
  /// Setup the units
  void Build();

  /// Return information about the active/current file being compiled.
  CompilerUnit &GetCurrentCompilerUnit() { return *currentUnit; }

  /// The the record of all files that were compiled.
  std::vector<CompilerUnit *> &GetCompilerUnits() { return units; }

public:
  int Parse();
  int Parse(bool check);
  int Check();
  int EmitIR();
  int EmitObject();
  int EmitAssembly();
  int EmitLibrary();
  /// Emit '.stonemodule'
  int EmitModuleOnly();
  int EmitBitCode();
};

Compiler::Implementation::Implementation(Compiler &compiler)
    : compiler(compiler) {}

Compiler::Implementation::~Implementation() {}

void Compiler::Implementation::Build() {}

int Compiler::Implementation::Parse(bool check) {
  for (const auto &input : compiler.GetCompilerOptions().inputs) {
    // stone::Parse::ParseSourceFile(su, compiler.GetPipeline());
    if (check) {
      if (!compiler.GetCompilerOptions().wholeModuleCheck) {
        //	stone::CheckSourceFile(su, compiler.GetPipeline());
      }
    }
  }
  if (check && compiler.GetCompilerOptions().wholeModuleCheck) {
    // stone::CheckWholeModule(su, compiler.GetPipeline());
  }
  return ret::ok;
}

int Compiler::Implementation::Parse() { return Parse(false); }
int Compiler::Implementation::Check() { return Parse(true); }

int Compiler::Implementation::EmitIR() {
  if (Check() == ret::err) {
    return ret::err;
  }
  llvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
                            compiler.compilerOpts.genOpts, /*TODO*/ {});
  return ret::ok;
}
int Compiler::Implementation::EmitObject() {
  if (EmitIR() == ret::err) {
    return ret::err;
  }
  bool status =
      stone::GenObject(llvmModule, compiler.GetCompilerOptions().genOpts,
                       compiler.GetTreeContext(), /*TODO*/ {});
  return ret::ok;
}

int Compiler::Implementation::EmitModuleOnly() { return ret::ok; }

int Compiler::Implementation::EmitLibrary() { return ret::ok; }

int Compiler::Implementation::EmitBitCode() { return ret::ok; }

int Compiler::Run(Compiler &compiler) {
  auto impl = llvm::make_unique<Compiler::Implementation>(compiler);
  impl->Build();
  switch (compiler.GetMode().GetKind()) {
  case ModeKind::Parse:
    return impl->Parse();
  case ModeKind::Check:
    return impl->Check();
  case ModeKind::EmitIR:
    return impl->EmitIR();
  case ModeKind::EmitObject:
    return impl->EmitObject();
  case ModeKind::EmitModuleOnly:
    return impl->EmitModuleOnly();
  case ModeKind::EmitBC:
    return impl->EmitBitCode();
  case ModeKind::EmitLibrary:
    return impl->EmitLibrary();
  default:
    llvm_unreachable("Invalide compiler mode.");
  }
  return ret::ok;
}

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, Pipeline *pipeline) {
  auto executablePath = stone::GetExecutablePath(arg0);

  Compiler compiler(pipeline);

  STONE_DEFER { compiler.Finish(); };

  if (compiler.Build(args)) {
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
  }
  return compiler.Run();
}
