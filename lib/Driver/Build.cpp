#include "stone/Core/Ret.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

void Driver::BuildActivities() {
  llvm::PrettyStackTraceString CrashInfo("Building compilation activities");

  if (mode.IsCompileOnly()) {
    BuildCompileActivities(*compilation.get());
  } else {
    BuildLinkActivity();
  }
}
void Driver::BuildCompileActivities(Compilation &compilation,
                                    CompilationActivity *le) {
  // Go through the files and build the compile activities

  for (const InputPair &input : profile.inputFiles) {
    // BuildCompileActivity(input);
    file::FileType inputType = input.first;
    const llvm::opt::Arg *inputArg = input.second;
    auto ie = compilation.CreateActivity<InputActivity>(*inputArg, inputType);

    switch (inputType) {
      case file::FileType::Stone: {
        assert(file::IsPartOfCompilation(inputType));
        BuildCompileActivity(compilation, ie, le);
      }
      default:
        break;
    }
  }
}

void Driver::BuildCompileActivity(Compilation &compilation, InputActivity *ie,
                                  CompilationActivity *le) {
  // if (profile.compileType == CompileType::MultipleInvocation) {
  //   } else if (profile.compileType == CompileType::SingleInvocation) {
  //}
  auto ce = compilation.CreateActivity<CompileActivity>(
      ie, profile.compilerOutputFileType);

  // Since we are here, let us build the jobs.
  BuildJobsForCompileActivity(compilation, ce);
}

void Driver::BuildJobsForCompileActivity(Compilation &compilation,
                                         const CompileActivity *ce) {}

void Driver::BuildLinkActivity() {
  // BuildCompileActivitys();
}
