#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

// Some job depend on other jobs -- For example, LinkJob
CompileJob::CompileJob(Compilation &compilation)
    : Job(JobType::Compile, compilation) {}

void CompileJob::BuildCmdOutput() {}

StoneTool::StoneTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Stone, toolChain) {
  toolOpts.canEmitIR = true;
}

Job *StoneTool::CreateJob(Compilation &compilation,
                          std::unique_ptr<CmdOutput> cmdOutput,
                          const OutputProfile &outputProfile) {
  auto job = llvm::make_unique<CompileJob>(compilation);
  Job *result = job.get();
  jobs.Add(std::move(job));
  return result;
}

Job *StoneTool::CreateJob(Compilation &compilation,
                          llvm::SmallVectorImpl<const Job *> &&deps,
                          std::unique_ptr<CmdOutput> cmdOutput,
                          const OutputProfile &outputProfile) {
  auto job = llvm::make_unique<CompileJob>(compilation);
  Job *result = job.get();
  jobs.Add(std::move(job));
  return result;
}

StoneTool::~StoneTool() {}
