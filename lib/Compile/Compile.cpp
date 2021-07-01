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

struct LangImplementation final {
  Compiler &compiler;
  SafeList<CompilableItem> compilables;

public:
  LangImplementation(Compiler &compiler) : compiler(compiler) {}
  ~LangImplementation() {}

public:
  int ExecuteCompilable(CompilableItem &compilable);

  void AddCompilable(std::unique_ptr<CompilableItem> compilable) {
    compilables.Add(std::move(compilable));
  }

  syn::SourceModuleFile *
  BuildSourceModuleFileForMainModule(SourceModuleFile::Kind kind,
                                     syn::Module &owner, SrcID srcID,
                                     bool isPrimary);

  std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                  file::File &input);

public:
  int Parse(CompilableItem &compilable, bool check);
  int Parse(CompilableItem &compilable);
  int Check(CompilableItem &compilable);
  int EmitIR(CompilableItem &compilable);
  int EmitObject(CompilableItem &compilable);
  int EmitAssembly(CompilableItem &compilable);
  int EmitLibrary(CompilableItem &compilable);
  int EmitModule(CompilableItem &compilable);
  int EmitBitCode(CompilableItem &compilable);
};

int LangImplementation::Parse(CompilableItem &compilable, bool check) {

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
int LangImplementation::Parse(CompilableItem &compilable) {
  return Parse(compilable, false);
}
int LangImplementation::Check(CompilableItem &compilable) {
  return Parse(compilable, true);
}

int LangImplementation::EmitIR(CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!Check(compilable)) {
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

int LangImplementation::EmitObject(CompilableItem &compilable) {

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
int LangImplementation::EmitAssembly(CompilableItem &compilable) {
  return ret::ok;
}
int LangImplementation::EmitLibrary(CompilableItem &compilable) {
  return ret::ok;
}
int LangImplementation::EmitModule(CompilableItem &compilable) {
  return ret::ok;
}
int LangImplementation::EmitBitCode(CompilableItem &compilable) {
  return ret::ok;
}

int LangImplementation::ExecuteCompilable(CompilableItem &compilable) {
  switch (compilable.GetCompiler().GetMode().GetType()) {
  case ModeType::Parse:
    return Parse(compilable);
  case ModeType::Check:
    return Check(compilable);
  case ModeType::EmitModule:
    return EmitModule(compilable);
  default:
    return EmitObject(compilable);
  }
}
syn::SourceModuleFile *LangImplementation::BuildSourceModuleFileForMainModule(
    SourceModuleFile::Kind kind, syn::Module &owner, SrcID srcID,
    bool isPrimary) {

  auto *sourceModuleFile = new (compiler.GetTreeContext())
      SourceModuleFile(kind, owner, srcID, isPrimary);

  return sourceModuleFile;
}

std::unique_ptr<CompilableItem>
LangImplementation::BuildCompilable(Compiler &compiler, file::File &input) {

  auto fileBuffer = compiler.GetFileMgr().getBufferForFile(input.GetName());
  if (!fileBuffer) {
    compiler.Error(0);
    return nullptr;
  }

  auto srcID = compiler.GetSrcMgr().CreateSrcID(std::move(*fileBuffer));
  auto sf = BuildSourceModuleFileForMainModule(
      SourceModuleFile::Kind::Library, *compiler.GetMainModule(), srcID, false);

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
    compiler.Error(0);
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  LangImplementation implemenation(compiler);
  for (auto &input : compiler.GetInputFiles()) {
    auto compilable = implemenation.BuildCompilable(compiler, input);
    if (compilable) {
      if (!implemenation.ExecuteCompilable(*compilable.get())) {
        break;
      }
      implemenation.AddCompilable(std::move(compilable));
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
