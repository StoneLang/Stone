#include "stone/Compile/Compile.h"
#include "stone/Compile/CompilableItem.h"

#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Frontend.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

class CompilerImpl final {
public:
  static int Parse(Compiler &compiler, CompilableItem &compilable);
  static int Check(Compiler &compiler, CompilableItem &compilable);
  static int EmitIR(Compiler &compiler, CompilableItem &compilable);
  static int EmitObject(Compiler &compiler, CompilableItem &compilable);
  static int EmitAssembly(Compiler &compiler, CompilableItem &compilable);
  static int EmitLibrary(Compiler &compiler, CompilableItem &compilable);
  static int EmitModuleOnly(Compiler &compiler, CompilableItem &compilable);
  static int EmitBitCode(Compiler &compiler, CompilableItem &compilable);
};

int CompilerImpl::Parse(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::Check(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::EmitIR(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::EmitObject(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::EmitAssembly(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::EmitLibrary(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::EmitModuleOnly(Compiler &compiler,
                                 CompilableItem &compilable) {
  return ret::ok;
}
int CompilerImpl::EmitBitCode(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}

int Compiler::Compile(Compiler &compiler) {

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return frontend::Parse(compiler);
  case ModeType::Check:
    return frontend::Check(compiler);
  case ModeType::EmitIR:
    return frontend::EmitIR(compiler);
  case ModeType::EmitObject:
    return frontend::EmitObject(compiler);
  case ModeType::EmitModuleOnly:
    return frontend::EmitModuleOnly(compiler);
  case ModeType::EmitBC:
    return frontend::EmitBitCode(compiler);
  case ModeType::EmitLibrary:
    return frontend::EmitLibrary(compiler);
  default:
    frontend::EmitObject(compiler);
  }
  return ret::ok;
}

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, PipelineEngine *pe) {

  auto executablePath = stone::GetExecutablePath(arg0);

  Compiler compiler(pe);
  STONE_DEFER { compiler.Finish(); };

  compiler.Init();
  if (compiler.Build(args)) {
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
  }
  return compiler.Run();
}
