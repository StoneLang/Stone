#include "stone/Compile/System.h"

#include "stone/Compile/Analysis.h"
#include "stone/Compile/Backend.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/Frontend.h"
#include "stone/Core/Module.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"
#include "stone/Session/Options.h"

using namespace stone;
using namespace stone::syntax;
using namespace stone::analysis;
using namespace stone::backend;

int System::Parse(bool check) {
  for (auto input : compiler.inputs) {
    // stone::analysis::Parse
    if (check) {
      if (!compiler.GetCompileOptions().analysisOpts.wholeModuleCheck) {
        // CheckSourceUnit();
      }
    }
  }
  if (check && compiler.GetCompileOptions().analysisOpts.wholeModuleCheck) {
    // CheckModule();
  }

  return ret::ok;
}
int System::CheckSourceUnit(syntax::SourceUnit *su) { return ret::ok; }
int System::CheckModule(syntax::Module *m) {}

int System::Parse() { return Parse(false); }
int System::Check() { return Parse(true); }

int System::EmitIR() {
  Check();
  llvmModule = analysis::GenIR(compiler.GetAnalysis().GetMainModule(), compiler,
                               compiler.compileOpts.genOpts, /*TODO*/ {});
  return ret::ok;
}
int System::EmitObject() {
  EmitIR();
  bool status =
      backend::GenObject(llvmModule, compiler.GetCompileOptions().genOpts,
                         compiler.GetAnalysis().GetASTContext(), /*TODO*/ {});
  return ret::ok;
}
