#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/System.h"
#include "stone/Core/Defer.h"
#include "stone/Core/Ret.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilePipeline *pipeline) {
  auto executablePath = stone::GetExecutablePath(arg0);

  Compiler compiler(pipeline);

  STONE_DEFER { compiler.Finish(); };

  if (compiler.Build(args)) {
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
  }
  System system(compiler);
  switch (compiler.GetMode().GetKind()) {
    case ModeKind::Parse:
      return system.Parse();
    case ModeKind::Check:
      return system.Check();
    case ModeKind::EmitIR:
      return system.EmitIR();
    case ModeKind::EmitObject:
      return system.EmitObject();
    default:
      break;
  }
  return ret::ok;
}
