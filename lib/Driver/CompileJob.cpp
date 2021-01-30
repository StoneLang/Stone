#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
CompileJob::CompileJob(Compilation& compilation)
    : Job(JobType::Compile, compilation) {}

void CompileJob::BuildCmdOutput() {}


StoneTool::StoneTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Stone, toolChain) {
  toolOpts.canEmitIR = true;
}

std::unique_ptr<Job> StoneTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  return llvm::make_unique<CompileJob>(compilation);
}

StoneTool::~StoneTool() {}

