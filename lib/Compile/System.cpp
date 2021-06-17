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
  static int ExecuteMode(Compiler &compiler, CompilableItem &compilable);

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

  // CompilableItem* Create<
};

static syn::SourceModuleFile *
CreateSourceModuleFileForMainModule(Compiler &compiler) {
  return nullptr;
}
static llvm::MemoryBuffer *CreateMemoryBuffer(Compiler &compiler,
                                              CompilableItem &compilable) {
  return nullptr;
}

static void BuildCompilables(Compiler &compiler, CompilableItems &compilables) {

  for (auto &input : compiler.GetCompilerOptions().GetInputs()) {
    // TODO: perf improvement
    std::unique_ptr<CompilableItem> compilable(
        new CompilableItem(InputFile(input, false, nullptr), compiler));

    compilables.entries.Add(std::move(compilable));
  }
}
int System::ExecuteMode(Compiler &compiler, CompilableItem &compilable) {

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return System::Parse(compiler, compilable);
  case ModeType::Check:
    return System::Check(compiler, compilable);
  default:
    return System::EmitObject(compiler, compilable);
  }
}
int System::Parse(Compiler &compiler, CompilableItem &compilable, bool check) {

  auto *sf = CreateSourceModuleFileForMainModule(compiler);
  assert(sf && "Could not create SourceModuleFile.");

  while (!compiler.Error()) {
    stone::ParseSourceModuleFile(*sf, compiler.GetSrcMgr(), compiler,
                                 compiler.GetPipelineEngine());
  }
  //   new
  //   (*compiler.GetTreeContext())SourceModuleFile(SourceFileKind::Library,
  //   compiler.GetMainModule(), CI->GetBufferID());

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

  compiler.GetCompilerContext().SetLLVMModule(llvmModule);

  return ret::ok;
}
int System::EmitObject(Compiler &compiler, CompilableItem &compilable) {

  /// Should be in EmitIR Scope
  if (!System::EmitIR(compiler, compilable)) {
    return ret::err;
  }

  bool status = stone::GenObject(
      compiler.GetCompilerContext().GetLLVMModule(),
      compiler.GetCompilerOptions().genOpts, compiler.GetTreeContext(),
      compilable.GetOutputFile()->GetName()); // TODO: Null check

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

int Compiler::Compile(Compiler &compiler) {

  if (compiler.GetInputs().empty()) {
    printf("No input files.\n"); // TODO: Use Diagnostics
    return ret::err;
  }
  assert(compiler.GetMode().IsCompilable() && "Invalid compile mode.");

  CompilableItems compilables;
  BuildCompilables(compiler, compilables);

  assert(!compilables.entries.empty() && "No items to compile.");

  for (auto &compilable : compilables.entries) {
    if (!System::ExecuteMode(compiler, compilable)) {
      // TODO: Prform some logging
      break;
    }
  }

  return ret::ok;
}
