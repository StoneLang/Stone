#include "stone/Compile/Frontend.h"
#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"

using namespace stone;

int frontend::Parse(Compiler &compiler, bool check) {

  // NOTE: Here
  // CompilingScope scope;
  // scope.Enter();
  // SourceFile su;

  for (const auto &input : compiler.GetCompilerOptions().inputs) {
    // stone::ParseSourceFile(su, pipeline);
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

int frontend::Parse(Compiler &compiler) {
  return frontend::Parse(compiler, false);
}

int frontend::Check(Compiler &compiler) {
  return frontend::Parse(compiler, true);
}
int frontend::EmitIR(Compiler &compiler) {

  if (frontend::Check(compiler) == ret::err) {
    return ret::err;
  }
  auto llvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
                                 compiler.compilerOpts.genOpts, /*TODO*/ {});

  compiler.GetCompilerContext().SetLLVMModule(llvmModule);

  return ret::ok;
}
int frontend::EmitObject(Compiler &compiler) {

  if (frontend::EmitIR(compiler) == ret::err) {
    return ret::err;
  }
  bool status = stone::GenObject(compiler.GetCompilerContext().GetLLVMModule(),
                                 compiler.GetCompilerOptions().genOpts,
                                 compiler.GetTreeContext(), /*TODO*/ {});

  return ret::ok;
}

int frontend::EmitModuleOnly(Compiler &compiler) { return ret::ok; }

int frontend::EmitLibrary(Compiler &compiler) { return ret::ok; }

int frontend::EmitBitCode(Compiler &compiler) { return ret::ok; }
