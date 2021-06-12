#include "stone/Compile/Frontend.h"
#include "stone/Analyze/Check.h"
#include "stone/Analyze/Parse.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Compiler.h"
#include "stone/Gen/Gen.h"

using namespace stone;
using namespace stone::sys;

Frontend::~Frontend() {}

int sys::Parse(Frontend &fe, bool check) {

  // NOTE: Here
  // CompilingScope scope;
  // scope.Enter();
  // SourceFile su;

  for (const auto &input : fe.GetCompiler().GetCompilerOptions().inputs) {
    // stone::ParseSourceFile(su, compiler.GetPipeline());
    if (check) {
      if (!fe.GetCompiler().GetCompilerOptions().wholeModuleCheck) {
        //	stone::CheckSourceFile(su, compiler.GetPipeline());
      }
    }
  }
  if (check && fe.GetCompiler().GetCompilerOptions().wholeModuleCheck) {
    // stone::CheckWholeModule(su, compiler.GetPipeline());
  }
  return ret::ok;
}

int sys::Parse(Frontend &fe) { return sys::Parse(fe, false); }

int sys::Check(Frontend &fe) { return sys::Parse(fe, true); }

int sys::EmitIR(Frontend &fe) {
  if (sys::Check(fe) == ret::err) {
    return ret::err;
  }
  auto llvmModule =
      stone::GenIR(fe.GetCompiler().GetMainModule(), fe.GetCompiler(),
                   fe.GetCompiler().compilerOpts.genOpts, /*TODO*/ {});

  fe.SetLLVMModule(llvmModule);

  return ret::ok;
}
int sys::EmitObject(Frontend &fe) {

  // if (sys::EmitIR(fe) == ret::err) {
  //  return ret::err;
  //}
  // bool status = stone::GenObject(
  //    fe.GetLLVMModule(), fe.GetCompiler().GetCompilerOptions().genOpts,
  //    fe.GetCompiler().GetTreeContext(), /*TODO*/ {});

  return ret::ok;
}

int sys::EmitModuleOnly(Frontend &fe) { return ret::ok; }

int sys::EmitLibrary(Frontend &fe) { return ret::ok; }

int sys::EmitBitCode(Frontend &fe) { return ret::ok; }
