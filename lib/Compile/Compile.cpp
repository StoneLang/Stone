#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Frontend.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;
using namespace stone::sys;

int Compiler::Run(Compiler &compiler) {

  Frontend fe(compiler);
  switch (compiler.GetMode().GetType()) {
  case ModeType::Parse:
    return sys::Parse(fe);
  case ModeType::Check:
    return sys::Check(fe);
  case ModeType::EmitIR:
    return sys::EmitIR(fe);
  case ModeType::EmitObject:
    return sys::EmitObject(fe);
  case ModeType::EmitModuleOnly:
    return sys::EmitModuleOnly(fe);
  case ModeType::EmitBC:
    return sys::EmitBitCode(fe);
  case ModeType::EmitLibrary:
    return sys::EmitLibrary(fe);
  default:
    sys::EmitObject(fe);
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
