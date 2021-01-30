#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"


using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
AssembleJob::AssembleJob(Compilation& compilation)
    : Job(JobType::Assemble, compilation) {}

void AssembleJob::BuildCmdOutput() {}

AssembleTool::AssembleTool(llvm::StringRef fullName, llvm::StringRef shortName,
                           const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Assemble, toolChain) {
  toolOpts.canAssemble = true;
}
std::unique_ptr<Job> AssembleTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  // Depend on the mode-type -- if you want to emit ir
  return llvm::make_unique<AssembleJob>(compilation);
}

AssembleTool::~AssembleTool() {}

