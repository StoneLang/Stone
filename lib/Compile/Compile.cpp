#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerTool.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

int Compiler::Run(Compiler &compiler) {

  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return stone::PerformParse(compiler);
  case ModeType::Check:
    return stone::PerformCheck(compiler);
  case ModeType::EmitIR:
    return stone::PerformEmitIR(compiler);
  case ModeType::EmitObject:
    return stone::PerformEmitObject(compiler);
  case ModeType::EmitModuleOnly:
    return stone::PerformEmitModuleOnly(compiler);
  case ModeType::EmitBC:
    return stone::PerformEmitBitCode(compiler);
  case ModeType::EmitLibrary:
    return stone::PerformEmitLibrary(compiler);
  default:
    stone::PerformEmitObject(compiler);
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
