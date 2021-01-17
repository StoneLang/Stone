#include "stone/Compile/Compile.h"

#include "stone/Compile/Analysis.h"
#include "stone/Compile/Backend.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Frontend.h"
#include "stone/Core/Defer.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"
#include "stone/Session/Options.h"

using namespace stone;
using namespace stone::syntax;
using namespace stone::analysis;
using namespace stone::backend;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilePipeline *pipeline) {
  Compiler compiler(pipeline);
  if (compiler.Build(args)) {
    assert(compiler.GetMode().IsCompileOnly() && "Not a compile mode");
    compiler.Run();
    compiler.Finish();
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
  }

  STONE_DEFER{};

  switch (compiler.GetMode().GetKind()) {
    case ModeKind::Parse:
    case ModeKind::Check: {
      if (compiler.GetDiagEngine().HasError()) {
        return ret::err;
      }
      return ret::ok;
    }
    default:
      break;
  }
  // We are not passing the compiler directly, we are pass stone::Context
  auto llvmModule =
      stone::analysis::GenIR(compiler.GetAnalysis().GetMainModule(), compiler,
                             compiler.compileOpts.genOpts, /*TODO*/ {});

  if (compiler.GetMode().GetKind() == ModeKind::EmitIR) {
    if (compiler.GetDiagEngine().HasError()) {
      return ret::err;
    }
    return ret::ok;
  }

  bool status = stone::backend::GenObject(
      llvmModule, compiler.compileOpts.genOpts,
      compiler.GetAnalysis().GetASTContext(), /*TODO*/ {});

  if (compiler.GetDiagEngine().HasError()) {
    return ret::err;
  }
  return ret::ok;
}
