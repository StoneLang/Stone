#include "stone/Compile/Compile.h"
#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

class CompilableItems final {
public:
  SafeList<CompilableItem> entries;
};

class CompilerImpl final {
public:
  static int Compile(Compiler &compiler, CompilableItem &compilable);

public:
  static int Parse(Compiler &compiler, CompilableItem &compilable);
  static int Parse(Compiler &compiler, CompilableItem &compilable, bool check);
  static int Check(Compiler &compiler, CompilableItem &compilable);
  static int EmitIR(Compiler &compiler, CompilableItem &compilable);
  static int EmitObject(Compiler &compiler, CompilableItem &compilable);
  static int EmitAssembly(Compiler &compiler, CompilableItem &compilable);
  static int EmitLibrary(Compiler &compiler, CompilableItem &compilable);
  static int EmitModuleOnly(Compiler &compiler, CompilableItem &compilable);
  static int EmitBitCode(Compiler &compiler, CompilableItem &compilable);

  // CompilableItem* Create<
};

int CompilerImpl::Parse(Compiler &compiler, CompilableItem &compilable,
                        bool check) {
  return ret::ok;
}
int CompilerImpl::Parse(Compiler &compiler, CompilableItem &compilable) {
  return CompilerImpl::Parse(compiler, compilable, false);
}
int CompilerImpl::Check(Compiler &compiler, CompilableItem &compilable) {
  return CompilerImpl::Parse(compiler, compilable, true);
}

int CompilerImpl::EmitIR(Compiler &compiler, CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!CompilerImpl::Parse(compiler, compilable, true)) {
    return ret::err;
  }
  /// Should be in Parse scope
  // if(compiler.GetCompilerContext().GetCompilingScope() !=
  // CompilingScopeType::Parsing) {
  //}
  // If we are here, then parse should have already been called.
  auto llvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
                                 compiler.compilerOpts.genOpts, /*TODO*/ {});

  compiler.GetCompilerContext().SetLLVMModule(llvmModule);

  return ret::ok;
}
int CompilerImpl::EmitObject(Compiler &compiler, CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!CompilerImpl::EmitIR(compiler, compilable)) {
    return ret::err;
  }
  bool status = stone::GenObject(
      compiler.GetCompilerContext().GetLLVMModule(),
      compiler.GetCompilerOptions().genOpts, compiler.GetTreeContext(),
      compilable.GetOutputFile()->GetName()); // TODO: Null check

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

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return CompilerImpl::Parse(compiler, compilable);
  case ModeType::Check:
    return CompilerImpl::Check(compiler, compilable);
  default:
    return CompilerImpl::EmitObject(compiler, compilable);
  }
}

static void BuildCompilables(Compiler &compiler, CompilableItems &compilables) {
  for (auto &input : compiler.GetCompilerOptions().GetInputs()) {
    // CompilableItem::Create(InputFile::Create(input.first, inut.second)):
  }
}

int Compiler::Compile(Compiler &compiler) {

  if (compiler.GetInputs().empty()) {
    printf("No input files.\n");
    return ret::err;
  }

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");

  CompilableItems compilables;
  BuildCompilables(compiler, compilables);

  for (auto &compilable : compilables.entries) {
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
