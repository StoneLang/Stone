#include "stone/Compile/CompilerTool.h"
#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"

using namespace stone;

int stone::PerformParse(Compiler &compiler, bool check) {

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

int stone::PerformParse(Compiler &compiler) {
  return stone::PerformParse(compiler, false);
}

int stone::PerformCheck(Compiler &compiler) {
  return stone::PerformParse(compiler, true);
}
int stone::PerformEmitIR(Compiler &compiler) {

  if (stone::PerformCheck(compiler) == ret::err) {
    return ret::err;
  }
  auto llvmModule = stone::GenIR(compiler.GetMainModule(), compiler,
                                 compiler.compilerOpts.genOpts, /*TODO*/ {});

  compiler.GetCompilerContext().SetLLVMModule(llvmModule);

  return ret::ok;
}
int stone::PerformEmitObject(Compiler &compiler) {

  // if (stone::EmitIR(compiler) == ret::err) {
  //  return ret::err;
  //}
  // bool status = stone::GenObject(
  //    compiler.GetLLVMModule(),
  //    compiler.GetCompiler().GetCompilerOptions().genOpts,
  //    compiler.GetCompiler().GetTreeContext(), /*TODO*/ {});

  return ret::ok;
}

int stone::PerformEmitModuleOnly(Compiler &compiler) { return ret::ok; }

int stone::PerformEmitLibrary(Compiler &compiler) { return ret::ok; }

int stone::PerformEmitBitCode(Compiler &compiler) { return ret::ok; }
