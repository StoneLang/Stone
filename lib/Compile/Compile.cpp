#include "stone/Compile/Compile.h"
#include "stone/Analysis/Check.h"
#include "stone/Analysis/Parse.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Utils/Defer.h"
#include "stone/Utils/Ret.h"

using namespace stone;
class CompilingScope final {
public:
  CompilingScope(const CompilingScope &) = delete;
  CompilingScope(CompilingScope &&) = delete;
  CompilingScope &operator=(const CompilingScope &) = delete;
  CompilingScope &operator=(CompilingScope &&) = delete;

public:
  CompilingScope() {}
  ~CompilingScope() {}

public:
  void Enter();
  void Exit();
};

class CompilingContext final {
  Compiler &compiler;

  // TODO: prevScope,
  CompilingScope scope;
  llvm::Module *llvmModule = nullptr;

private:
  CompilingContext(const CompilingContext &) = delete;
  CompilingContext(CompilingContext &&) = delete;
  CompilingContext &operator=(const CompilingContext &) = delete;
  CompilingContext &operator=(CompilingContext &&) = delete;

public:
  CompilingContext(Compiler &compiler) : compiler(compiler) {}
  ~CompilingContext();

public:
  Compiler &GetCompiler() { return compiler; }
  CompilingScope &GetScope() { return scope; }
  void SetLLVMModule(llvm::Module *m) { llvmModule = m; }

  llvm::Module *GetLLVMModule() {
    assert(llvmModule && "No LLVM Module");
    return llvmModule;
  }
};
CompilingContext::~CompilingContext() {}

static int ActOnParse(CompilingContext &cc);
static int ActOnarse(CompilingContext &cc, bool check);
static int ActOnCheck(CompilingContext &cc);
static int ActOnEmitIR(CompilingContext &cc);
static int ActOnEmitObject(CompilingContext &cc);
static int ActOnEmitAssembly(CompilingContext &cc);
static int ActOnEmitLibrary(CompilingContext &cc);
static int ActOnEmitModuleOnly(CompilingContext &cc);
static int ActOnEmitBitCode(CompilingContext &cc);

/*
void ActOnBuild() {

  // Create CompilerUnits
  for (const auto &input : compiler.GetCompilerOptions().inputs) {
  }
}
*/
int ActOnParse(CompilingContext &cc, bool check) {
  // CompilingScope scope;
  // scope.Enter();

  for (const auto &input : cc.GetCompiler().GetCompilerOptions().inputs) {
    // stone::Parse::ParseSourceFile(su, compiler.GetPipeline());
    if (check) {
      if (!cc.GetCompiler().GetCompilerOptions().wholeModuleCheck) {
        //	stone::CheckSourceFile(su, compiler.GetPipeline());
      }
    }
  }
  if (check && cc.GetCompiler().GetCompilerOptions().wholeModuleCheck) {
    // stone::CheckWholeModule(su, compiler.GetPipeline());
  }
  return ret::ok;
}

int ActOnParse(CompilingContext &cc) { return ActOnParse(cc, false); }
int ActOnCheck(CompilingContext &cc) { return ActOnParse(cc, true); }

int ActOnEmitIR(CompilingContext &cc) {
  if (ActOnCheck(cc) == ret::err) {
    return ret::err;
  }
  auto llvmModule =
      stone::GenIR(cc.GetCompiler().GetMainModule(), cc.GetCompiler(),
                   cc.GetCompiler().compilerOpts.genOpts, /*TODO*/ {});

  cc.SetLLVMModule(llvmModule);

  return ret::ok;
}
int ActOnEmitObject(CompilingContext &cc) {
  if (ActOnEmitIR(cc) == ret::err) {
    return ret::err;
  }
  bool status = stone::GenObject(
      cc.GetLLVMModule(), cc.GetCompiler().GetCompilerOptions().genOpts,
      cc.GetCompiler().GetTreeContext(), /*TODO*/ {});
  return ret::ok;
}

int ActOnEmitModuleOnly(CompilingContext &cc) { return ret::ok; }

int ActOnEmitLibrary(CompilingContext &cc) { return ret::ok; }

int ActOnEmitBitCode(CompilingContext &cc) { return ret::ok; }

int Compiler::Run(Compiler &compiler) {

  CompilingContext cc(compiler);

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return ActOnParse(cc);
  case ModeType::Check:
    return ActOnCheck(cc);
  case ModeType::EmitIR:
    return ActOnEmitIR(cc);
  case ModeType::EmitObject:
    return ActOnEmitObject(cc);
  case ModeType::EmitModuleOnly:
    return ActOnEmitModuleOnly(cc);
  case ModeType::EmitBC:
    return ActOnEmitBitCode(cc);
  case ModeType::EmitLibrary:
    return ActOnEmitLibrary(cc);
  default:
    llvm_unreachable("Unknown compiler mode.");
  }
  return ret::ok;
}

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, Pipeline *pipeline) {

  auto executablePath = stone::GetExecutablePath(arg0);

  Compiler compiler(pipeline);

  STONE_DEFER { compiler.Finish(); };

  if (compiler.Build(args)) {
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
  }
  return compiler.Run();
}
