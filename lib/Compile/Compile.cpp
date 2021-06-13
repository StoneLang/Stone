#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Frontend.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

class CompilerImpl final {
public:
  static int Compile(Compiler &compiler, CompilableItem &compilable);

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
int CompilerImpl::Compile(Compiler &compiler, CompilableItem &compilable) {

  /// All modes require parsing.
  if (!CompilerImpl::Parse(compiler, compilable)) {
    return ret::err;
  }
  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return ret::ok;
  case ModeType::Check:
    return CompilerImpl::Check(compiler, compilable);
  default:
    return CompilerImpl::EmitObject(compiler, compilable);
  }
}

static void BuildCompilables(Compiler &compiler,
                             SafeList<CompilableItem> &compilables) {

  for (const auto &input : compiler.GetCompilerOptions().inputs) {
  }
}

int Compiler::Compile(Compiler &compiler) {

  if (!compiler.GetMode().CanCompile()) {
    return ret::err;
  }

  SafeList<CompilableItem> compilables;
  BuildCompilables(compiler, compilables);

  for (auto &compilable : compilables) {
    if (!CompilerImpl::Compile(compiler, compilable)) {

      // TODO: Prform some logging
      break;
    }
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
