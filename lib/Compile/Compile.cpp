
#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"
#include "stone/Session/ExecutablePath.h"

#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Ret.h"
#include "stone/Gen/Gen.h"
#include "stone/Syntax/Module.h"

using namespace stone;

static syn::SourceModuleFile *
BuildSourceModuleFileForMainModule(Compiler &compiler,
                                   CompilableItem &compilable) {

  return nullptr;
}
static int Parse(CompilableItem &compilable, bool check) {

  while (!compilable.GetCompiler().HasError()) {
    stone::ParseSourceModuleFile(compilable.GetSourceModuleFile(),
                                 compilable.GetCompiler().GetSyntax(),
                                 compilable.GetCompiler().GetPipelineEngine());
    if (compilable.GetCompiler().HasError()) {
      return ret::err;
    }
  }
  return ret::ok;
}
static int Parse(CompilableItem &compilable) {
  return Parse(compilable, false);
}
static int Check(CompilableItem &compilable) { return Parse(compilable, true); }

static int EmitIR(CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!Parse(compilable, true)) {
    return ret::err;
  }
  /// Should be in Parse scope
  // if(compiler.GetCompilerContext().GetCompilingScope() !=
  // CompilingScopeType::Parsing) {
  //}
  // If we are here, then parse should have already been called.
  auto llvmModule = stone::GenIR(compilable.GetCompiler().GetMainModule(),
                                 compilable.GetCompiler(),
                                 compilable.GetCompiler().compilerOpts.genOpts,
                                 compilable.GetOutputFile());

  if (compilable.GetCompiler().HasError()) {
    return ret::err;
  }
  compilable.GetCompiler().GetCompilerContext().SetLLVMModule(llvmModule);
  return ret::ok;
}

static int EmitObject(CompilableItem &compilable) {

  if (!compilable.GetCompiler().GetMode().CanOutput()) {
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

  if (!stone::GenObject(
          compilable.GetCompiler().GetCompilerContext().GetLLVMModule(),
          compilable.GetCompiler().GetCompilerOptions().genOpts,
          compilable.GetCompiler().GetTreeContext(),
          compilable.GetOutputFile())) {
    return ret::err;
  }

  return ret::ok;
}
static int EmitAssembly(CompilableItem &compilable) { return ret::ok; }
static int EmitLibrary(CompilableItem &compilable) { return ret::ok; }
static int EmitModuleOnly(CompilableItem &compilable) { return ret::ok; }
static int EmitBitCode(CompilableItem &compilable) { return ret::ok; }

static int ExecuteCompilable(CompilableItem &compilable) {
  switch (compilable.GetCompiler().GetMode().GetType()) {
  case ModeType::Parse:
    return Parse(compilable);
  case ModeType::Check:
    return Check(compilable);
  default:
    return EmitObject(compilable);
  }
}
static std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                       file::File &input) {

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

int Compiler::Run(Compiler &compiler) {

  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");
  if (compiler.GetInputFiles().empty()) {
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  for (auto &input : compiler.GetInputFiles()) {
    auto compilable = BuildCompilable(compiler, input);
    if (compilable) {
      if (!ExecuteCompilable(*compilable.get())) {
        break;
      }
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
    if (compiler.HasError()) {
      return ret::err;
    }
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
  }
  return compiler.Run();
}
