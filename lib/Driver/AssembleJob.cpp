#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
AssembleJob::AssembleJob(Compilation &compilation)
    : Job(JobType::Assemble, compilation) {}

void AssembleJob::BuildCmdOutput() {}

AssembleTool::AssembleTool(llvm::StringRef fullName, llvm::StringRef shortName,
                           const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Assemble, toolChain) {
  toolOpts.canAssemble = true;
}
Job *AssembleTool::CreateJob(Compilation &compilation,
                             llvm::SmallVectorImpl<const Job *> &&deps,
                             std::unique_ptr<CmdOutput> cmdOutput,
                             const OutputProfile &outputProfile) {


	auto job = llvm::make_unique<AssembleJob>(compilation);
	Job *result = job.get(); 
  jobs.Add(std::move(job));
  return result;

}

AssembleTool::~AssembleTool() {}
