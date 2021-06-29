#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"
#include "stone/Syntax/Module.h"

using namespace stone;

class CompilerInternal final {
  SafeList<CompilableItem> compilableItems;

public:
  static int ResolveImports(Compiler &compiler);
  static int ExecuteCompilable(Compiler &compiler, CompilableItem &compilable);

  static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                         file::File &input);
  static syn::SourceModuleFile *
  BuildSourceModuleFileForMainModule(Compiler &compiler,
                                     CompilableItem &compilable);

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
};
int CompilerInternal::Parse(Compiler &compiler, CompilableItem &compilable,
                            bool check) {

  while (!compiler.HasError()) {
    stone::ParseSourceModuleFile(compilable.GetSourceModuleFile(),
                                 compiler.GetSyntax(),
                                 compiler.GetPipelineEngine());
    if (compiler.HasError())
      return ret::err;
  }
  return ret::ok;
}
int CompilerInternal::Parse(Compiler &compiler, CompilableItem &compilable) {
  return CompilerInternal::Parse(compiler, compilable, false);
}
int CompilerInternal::Check(Compiler &compiler, CompilableItem &compilable) {
  return CompilerInternal::Parse(compiler, compilable, true);
}

int CompilerInternal::EmitIR(Compiler &compiler, CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!CompilerInternal::Parse(compiler, compilable, true)) {
    return ret::err;
  }
  /// Should be in Parse scope
  // if(compiler.GetCompilerContext().GetCompilingScope() !=
  // CompilingScopeType::Parsing) {
  //}
  // If we are here, then parse should have already been called.
  auto llvmModule =
      stone::GenIR(compiler.GetMainModule(), compiler,
                   compiler.compilerOpts.genOpts, compilable.GetOutputFile());

  if (compiler.HasError()) {
    return ret::err;
  }
  compiler.GetCompilerContext().SetLLVMModule(llvmModule);
  return ret::ok;
}
int CompilerInternal::EmitObject(Compiler &compiler,
                                 CompilableItem &compilable) {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }
  if (!compilable.GetOutputFile()) {
    return ret::err;
  }
  /// Should be in EmitIR Scope
  if (!CompilerInternal::EmitIR(compiler, compilable)) {
    return ret::err;
  }

  if (!stone::GenObject(compiler.GetCompilerContext().GetLLVMModule(),
                        compiler.GetCompilerOptions().genOpts,
                        compiler.GetTreeContext(),
                        compilable.GetOutputFile())) {
    return ret::err;
  }

  return ret::ok;
}
int CompilerInternal::EmitAssembly(Compiler &compiler,
                                   CompilableItem &compilable) {
  return ret::ok;
}
int CompilerInternal::EmitLibrary(Compiler &compiler,
                                  CompilableItem &compilable) {
  return ret::ok;
}
int CompilerInternal::EmitModuleOnly(Compiler &compiler,
                                     CompilableItem &compilable) {
  return ret::ok;
}
int CompilerInternal::EmitBitCode(Compiler &compiler,
                                  CompilableItem &compilable) {
  return ret::ok;
}

std::unique_ptr<CompilableItem>
CompilerInternal::BuildCompilable(Compiler &compiler, file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    // TODO: compiler.Diagnose();
    return nullptr;
  }
  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));

  // Use the srcID to create the SourceModuleFile
  SourceModuleFile *sf = nullptr;
  // CompilerInternal::BuildSourceModuleFileForMainModule(compiler,
  // compilable);
  assert(sf && "Could not create SourceModuleFile");

  std::unique_ptr<CompilableItem> compilable(
      new CompilableItem(CompilableFile(input, false), compiler, *sf));

  // TODO: May want to do tis later
  if (compilable->CanOutput()) {
    compilable->CreateOutputFile();
  }
  return compilable;
}
int CompilerInternal::ExecuteCompilable(Compiler &compiler,
                                        CompilableItem &compilable) {

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return CompilerInternal::Parse(compiler, compilable);
  case ModeType::Check:
    return CompilerInternal::Check(compiler, compilable);
  default:
    return CompilerInternal::EmitObject(compiler, compilable);
  }
}
int Compiler::Compile(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  for (auto &input : compiler.GetInputFiles()) {
    auto compilable = CompilerInternal::BuildCompilable(compiler, input);
    if (compilable) {
      if (!CompilerInternal::ExecuteCompilable(compiler, *compilable.get())) {
        break;
      }
    }
  }
  return ret::ok;
}
