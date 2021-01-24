#include "stone/Compile/Compile.h"

#include "stone/Compile/Compiler.h"
#include "stone/Compile/Gen.h"
#include "stone/Core/Defer.h"
#include "stone/Core/Ret.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

class Compiler::Implementation final {
  Compiler &compiler;
  llvm::Module *llvmModule = nullptr;

 public:
  Implementation(Compiler &compiler) : compiler(compiler) {}
  ~Implementation() {}

 public:
  int Parse();
  int Parse(bool check);
  int Check();
  int EmitIR();
  int EmitObject();
  int EmitAssembly();
};

int Compiler::Implementation::Parse(bool check) {
  for (auto input : compiler.GetCompileOptions().inputs) {
    // stone::analysis::Parse
    if (check) {
      if (!compiler.GetCompileOptions().wholeModuleCheck) {
        //	stone::CheckSourceUnit();
      }
    }
  }
  if (check && compiler.GetCompileOptions().wholeModuleCheck) {
    // stone::CheckWholeModule();
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
                            compiler.compileOpts.genOpts, /*TODO*/ {});
  return ret::ok;
}
int Compiler::Implementation::EmitObject() {
  if (EmitIR() == ret::err) {
    return ret::err;
  }
  bool status =
      stone::GenObject(llvmModule, compiler.GetCompileOptions().genOpts,
                       compiler.GetASTContext(), /*TODO*/ {});
  return ret::ok;
}
int Compiler::Run(Compiler &compiler) {
  auto implementation = llvm::make_unique<Compiler::Implementation>(compiler);

  switch (compiler.GetMode().GetKind()) {
    case ModeKind::Parse:
      return implementation->Parse();
    case ModeKind::Check:
      return implementation->Check();
    case ModeKind::EmitIR:
      return implementation->EmitIR();
    case ModeKind::EmitObject:
      return implementation->EmitObject();
    default:
      break;
  }
  return ret::ok;
}

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilePipeline *pipeline) {
  auto executablePath = stone::GetExecutablePath(arg0);

  Compiler compiler(pipeline);

  STONE_DEFER { compiler.Finish(); };

  if (compiler.Build(args)) {
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
    return compiler.Run();
  }
  return ret::ok;
}
