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

class CompilableItems final {
public:
  SafeList<CompilableItem> entries;
};

class System final {
public:
  static int ExecuteCompilable(Compiler &compiler, CompilableItem &compilable);

public:
  static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                         file::File &input);
  static syn::SourceModuleFile *
  BuildSourceModuleFileForMainModule(Compiler &compiler,
                                     CompilableItem &compilable);

public:
  // Mode operations
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
int System::Parse(Compiler &compiler, CompilableItem &compilable, bool check) {

  while (!compiler.Error()) {
    stone::ParseSourceModuleFile(compilable.GetSourceModuleFile(),
                                 compiler.GetSrcMgr(), compiler,
                                 compiler.GetPipelineEngine());
    if (compiler.Error())
      return ret::err;
  }
  return ret::ok;
}
int System::Parse(Compiler &compiler, CompilableItem &compilable) {
  return System::Parse(compiler, compilable, false);
}
int System::Check(Compiler &compiler, CompilableItem &compilable) {
  return System::Parse(compiler, compilable, true);
}

int System::EmitIR(Compiler &compiler, CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!System::Parse(compiler, compilable, true)) {
    return ret::err;
  }
  /// Should be in Parse scope
  // if(compiler.GetCompilerContext().GetCompilingScope() !=
  // CompilingScopeType::Parsing) {
  //}
  // If we are here, then parse should have already been called.
  auto llvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
                                 compiler.compilerOpts.genOpts, /*TODO*/ {});

  if (compiler.Error())
    return ret::err;

  compiler.GetCompilerContext().SetLLVMModule(llvmModule);

  return ret::ok;
}
int System::EmitObject(Compiler &compiler, CompilableItem &compilable) {

  if (!compiler.GetMode().CanOutput()) {
    return ret::err;
  }
  if (!compilable.GetOutputFile()) {
    return ret::err;
  }
  /// Should be in EmitIR Scope
  if (!System::EmitIR(compiler, compilable)) {
    return ret::err;
  }

  if (!stone::GenObject(compiler.GetCompilerContext().GetLLVMModule(),
                        compiler.GetCompilerOptions().genOpts,
                        compiler.GetTreeContext(),
                        compilable.GetOutputFile()->GetFile().GetName())) {
    return ret::err;
  }

  return ret::ok;
}
int System::EmitAssembly(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int System::EmitLibrary(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int System::EmitModuleOnly(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}
int System::EmitBitCode(Compiler &compiler, CompilableItem &compilable) {
  return ret::ok;
}

std::unique_ptr<CompilableItem> System::BuildCompilable(Compiler &compiler,
                                                        file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    // TODO: log
    return nullptr;
  }
  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));

  // Use the srcID to create the SourceModuleFile
  SourceModuleFile *sf = nullptr;
  // System::BuildSourceModuleFileForMainModule(compiler,
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
int System::ExecuteCompilable(Compiler &compiler, CompilableItem &compilable) {

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return System::Parse(compiler, compilable);
  case ModeType::Check:
    return System::Check(compiler, compilable);
  default:
    return System::EmitObject(compiler, compilable);
  }
}
int Compiler::Compile(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  for (auto &input : compiler.GetInputFiles()) {
    auto compilable = System::BuildCompilable(compiler, input);
    if (!System::ExecuteCompilable(compiler, *compilable.get())) {
      break;
    }
  }
  return ret::ok;
}
