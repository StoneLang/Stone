
#include "stone/Compile/System.h"
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

static syn::SourceModuleFile *
BuildSourceModuleFileForMainModule(Compiler &compiler,
                                   CompilableItem &compilable) {

  return nullptr;
}
int System::Parse(CompilableItem &compilable, bool check) {

  while (!compiler.HasError()) {
    stone::ParseSourceModuleFile(compilable.GetSourceModuleFile(),
                                 compiler.GetSyntax(),
                                 compiler.GetPipelineEngine());
    if (compiler.HasError()) {
      return ret::err;
    }
  }
  return ret::ok;
}
int System::Parse(CompilableItem &compilable) {
  return Parse(compilable, false);
}
int System::Check(CompilableItem &compilable) {
  return Parse(compilable, true);
}

int System::EmitIR(CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!Parse(compilable, true)) {
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
int System::EmitObject(CompilableItem &compilable) {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }
  compilable.CreateOutputFile();

  if (!compilable.GetOutputFile()) {
    return ret::err;
  }
  /// Should be in EmitIR Scope
  if (!EmitIR(compilable)) {
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
int System::EmitAssembly(CompilableItem &compilable) { return ret::ok; }
int System::EmitLibrary(CompilableItem &compilable) { return ret::ok; }
int System::EmitModuleOnly(CompilableItem &compilable) { return ret::ok; }
int System::EmitBitCode(CompilableItem &compilable) { return ret::ok; }

std::unique_ptr<CompilableItem> System::BuildCompilable(file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    // TODO: compiler.Diagnose();
    return nullptr;
  }
  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));

  // Use the srcID to create the SourceModuleFile
  SourceModuleFile *sf = nullptr;
  // sys::BuildSourceModuleFileForMainModule(compiler,
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
int System::ExecuteCompilable(CompilableItem &compilable) {

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return Parse(compilable);
  case ModeType::Check:
    return Check(compilable);
  default:
    return EmitObject(compilable);
  }
}
int Compiler::Compile(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  System system(compiler);

  for (auto &input : compiler.GetInputFiles()) {
    auto compilable = system.BuildCompilable(input);
    if (compilable) {
      if (!system.ExecuteCompilable(*compilable.get())) {
        break;
      }
    }
  }
  return ret::ok;
}
