#include "stone/Analysis/Check.h"
#include "stone/Analysis/Parse.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"
#include "stone/Session/ExecutablePath.h"
#include "stone/Utils/Defer.h"
#include "stone/Utils/Ret.h"

using namespace stone;

class CompilerImpl final {
  Compiler &compiler;
  llvm::Module *llvmModule = nullptr;

public:
  CompilerImpl(Compiler &compiler);
  ~CompilerImpl();

public:
  /// Setup the units
  void Build();

public:
  int Parse();
  int Parse(bool check);
  int Check();
  int EmitIR();
  int EmitObject();
  int EmitAssembly();
  int EmitLibrary();
  int EmitModuleOnly();
  int EmitBitCode();
};

CompilerImpl::CompilerImpl(Compiler &compiler) : compiler(compiler) {}

CompilerImpl::~CompilerImpl() {}

void CompilerImpl::Build() {

  // Create CompilerUnits
  for (const auto &input : compiler.GetCompilerOptions().inputs) {
  }
}

int CompilerImpl::Parse(bool check) {
  for (const auto &input : compiler.GetCompilerOptions().inputs) {
    // stone::Parse::ParseSourceFile(su, compiler.GetPipeline());
    if (check) {
      if (!compiler.GetCompilerOptions().wholeModuleCheck) {
        //	stone::CheckSourceFile(su, compiler.GetPipeline());
      }
    }
  }
  if (check && compiler.GetCompilerOptions().wholeModuleCheck) {
    // stone::CheckWholeModule(su, compiler.GetPipeline());
  }
  return ret::ok;
}

int CompilerImpl::Parse() { return Parse(false); }
int CompilerImpl::Check() { return Parse(true); }

int CompilerImpl::EmitIR() {
  if (Check() == ret::err) {
    return ret::err;
  }
  llvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
                            compiler.compilerOpts.genOpts, /*TODO*/ {});
  return ret::ok;
}
int CompilerImpl::EmitObject() {
  if (EmitIR() == ret::err) {
    return ret::err;
  }
  bool status =
      stone::GenObject(llvmModule, compiler.GetCompilerOptions().genOpts,
                       compiler.GetTreeContext(), /*TODO*/ {});
  return ret::ok;
}

int CompilerImpl::EmitModuleOnly() { return ret::ok; }

int CompilerImpl::EmitLibrary() { return ret::ok; }

int CompilerImpl::EmitBitCode() { return ret::ok; }

int Compiler::Run(Compiler &compiler) {
  CompilerImpl impl(compiler);
  impl.Build();

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return impl.Parse();
  case ModeType::Check:
    return impl.Check();
  case ModeType::EmitIR:
    return impl.EmitIR();
  case ModeType::EmitObject:
    return impl.EmitObject();
  case ModeType::EmitModuleOnly:
    return impl.EmitModuleOnly();
  case ModeType::EmitBC:
    return impl.EmitBitCode();
  case ModeType::EmitLibrary:
    return impl.EmitLibrary();
  default:
    llvm_unreachable("Invalide compiler mode.");
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
