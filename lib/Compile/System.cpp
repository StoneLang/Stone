#include "stone/Compile/System.h"

#include "stone/Compile/Analysis.h"
#include "stone/Compile/Analyze.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Gen.h"
#include "stone/Core/Module.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"
#include "stone/Session/Options.h"

using namespace stone;
using namespace stone::syntax;
using namespace stone::analysis;

int System::Parse(bool check) {
  for (auto input : compiler.inputs) {
    // stone::analysis::Parse
    if (check) {
      if (!compiler.GetCompileOptions().analysisOpts.wholeModuleCheck) {
			//	stone::CheckSourceUnit();
      }
    }
  }
  if (check && compiler.GetCompileOptions().analysisOpts.wholeModuleCheck) {
    // stone::CheckWholeModule();
  }

  return ret::ok;
}

int System::Parse() { return Parse(false); }
int System::Check() { return Parse(true); }

int System::EmitIR() {
  Check();
  llvmModule = stone::GenIR(compiler.GetAnalysis().GetMainModule(), compiler,
                            compiler.compileOpts.genOpts, /*TODO*/ {});

  return ret::ok;
}
int System::EmitObject() {
  EmitIR();
  bool status =
      stone::GenObject(llvmModule, compiler.GetCompileOptions().genOpts,
                       compiler.GetAnalysis().GetASTContext(), /*TODO*/ {});
  return ret::ok;
}

