#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

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
