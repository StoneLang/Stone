#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

// Some jobs only consume inputs -- For example, LinkJob
LinkJob::LinkJob(JobType jobType, Compilation &compilation, bool requiresLTO,
                 LinkType linkType)
    : Job(jobType, compilation), requiresLTO(requiresLTO), linkType(linkType) {}

// Some jobs only consume inputs -- For example, LinkJob
StaticLinkJob::StaticLinkJob(Compilation &compilation, bool requiresLTO,
                             LinkType linkType)
    : LinkJob(JobType::StaticLink, compilation, requiresLTO, linkType) {}

void StaticLinkJob::BuildCmdOutput() {}

// Some jobs only consume inputs -- For example, LinkJob
DynamicLinkJob::DynamicLinkJob(Compilation &compilation, bool requiresLTO,
                               LinkType linkType)
    : LinkJob(JobType::DynamicLink, compilation, requiresLTO, linkType) {}

void DynamicLinkJob::BuildCmdOutput() {}

ClangTool::ClangTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain, LinkType linkType)
    : LinkTool(fullName, shortName, ToolType::Clang, toolChain, linkType) {
  toolOpts.canEmitIR = true;
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}

Job *ClangTool::CreateJob(Compilation &compilation,
                          std::unique_ptr<CmdOutput> cmdOutput,
                          const OutputProfile &outputProfile) {
  return LinkTool::CreateJob(compilation, std::move(cmdOutput), outputProfile);
}

Job *ClangTool::CreateJob(Compilation &compilation,
                          llvm::SmallVectorImpl<const Job *> &&deps,
                          std::unique_ptr<CmdOutput> cmdOutput,
                          const OutputProfile &outputProfile) {
  return LinkTool::CreateJob(compilation, std::move(deps), std::move(cmdOutput),
                             outputProfile);
}

ClangTool::~ClangTool() {}

GCCTool::GCCTool(llvm::StringRef fullName, llvm::StringRef shortName,
                 const ToolChain &toolChain, LinkType linkType)
    : LinkTool(fullName, shortName, ToolType::GCC, toolChain, linkType) {
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}

Job *GCCTool::CreateJob(Compilation &compilation,
                        std::unique_ptr<CmdOutput> cmdOutput,
                        const OutputProfile &outputProfile) {
  // NOTE: For the time being.
  return LinkTool::CreateJob(compilation, std::move(cmdOutput), outputProfile);
}

Job *GCCTool::CreateJob(Compilation &compilation,
                        llvm::SmallVectorImpl<const Job *> &&deps,
                        std::unique_ptr<CmdOutput> cmdOutput,
                        const OutputProfile &outputProfile) {
  // NOTE: For the time being.
  return LinkTool::CreateJob(compilation, std::move(deps), std::move(cmdOutput),
                             outputProfile);
}

GCCTool::~GCCTool() {}

LinkTool::LinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                   ToolType toolType, const ToolChain &toolChain,
                   LinkType linkType)
    : Tool(fullName, shortName, toolType, toolChain), linkType(linkType) {
  toolOpts.canLink = true;
}

Job *LinkTool::CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) {
  Job *result = nullptr;
  // Depend on the mode-type -- if you want to emit ir
  if (linkType == LinkType::DynamicLibrary) {
    auto job = llvm::make_unique<DynamicLinkJob>(
        compilation, compilation.GetDriver().GetOutputProfile().RequiresLTO(),
        compilation.GetDriver().GetOutputProfile().linkType);
    result = job.get();
    jobs.Add(std::move(job));

  } else {
    auto job = llvm::make_unique<StaticLinkJob>(
        compilation, compilation.GetDriver().GetOutputProfile().RequiresLTO(),
        compilation.GetDriver().GetOutputProfile().linkType);
    result = job.get();
    jobs.Add(std::move(job));
  }
  return result;
}

Job *LinkTool::CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) {
  Job *result = nullptr;
  // Depend on the mode-type -- if you want to emit ir
  if (linkType == LinkType::DynamicLibrary) {
    auto job = llvm::make_unique<DynamicLinkJob>(
        compilation, compilation.GetDriver().GetOutputProfile().RequiresLTO(),
        compilation.GetDriver().GetOutputProfile().linkType);
    result = job.get();
    jobs.Add(std::move(job));

  } else {
    auto job = llvm::make_unique<StaticLinkJob>(
        compilation, compilation.GetDriver().GetOutputProfile().RequiresLTO(),
        compilation.GetDriver().GetOutputProfile().linkType);
    result = job.get();
    jobs.Add(std::move(job));
  }
  return result;
}

LinkTool::~LinkTool() {}

LLDLinkTool::LLDLinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                         const ToolChain &toolChain, LinkType linkType)
    : LinkTool(fullName, shortName, ToolType::LLD, toolChain, linkType) {}

LLDLinkTool::~LLDLinkTool() {}

LDLinkTool::LDLinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                       const ToolChain &toolChain, LinkType linkType)
    : LinkTool(fullName, shortName, ToolType::LD, toolChain, linkType) {}

LDLinkTool::~LDLinkTool() {}
