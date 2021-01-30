#include "stone/Driver/ToolChain.h"

#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::driver;

Tool::Tool(llvm::StringRef fullName, llvm::StringRef shortName,
           ToolType toolType, const ToolChain &toolChain)
    : fullName(fullName),
      shortName(shortName),
      toolType(toolType),
      toolChain(toolChain) {}

Tool::~Tool() {}

StoneTool::StoneTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Stone, toolChain) {
  toolOpts.canEmitIR = true;
}

std::unique_ptr<Job> StoneTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  return nullptr;  // llvm::make_unique<CompileJob>(
}

StoneTool::~StoneTool() {}

ClangTool::ClangTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Clang, toolChain) {
  toolOpts.canEmitIR = true;
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}
std::unique_ptr<Job> ClangTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  return nullptr;
}

ClangTool::~ClangTool() {}

GCCTool::GCCTool(llvm::StringRef fullName, llvm::StringRef shortName,
                 const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::GCC, toolChain) {
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}
std::unique_ptr<Job> GCCTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  return nullptr;
}

GCCTool::~GCCTool() {}

LinkTool::LinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                   ToolType toolType, const ToolChain &toolChain,
                   LinkType linkType)
    : Tool(fullName, shortName, toolType, toolChain), linkType(linkType) {
  toolOpts.canLink = true;
}
std::unique_ptr<Job> LinkTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  return nullptr;
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

AssembleTool::AssembleTool(llvm::StringRef fullName, llvm::StringRef shortName,
                           const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Assemble, toolChain) {
  toolOpts.canAssemble = true;
}
std::unique_ptr<Job> AssembleTool::CreateJob(
    Compilation &compilation, llvm::SmallVectorImpl<const Job *> &&deps,
    std::unique_ptr<CmdOutput> cmdOutput,
    const OutputProfile &outputProfile) const {
  return nullptr;
}

AssembleTool::~AssembleTool() {}

ToolChain::ToolChain(const Driver &driver, const llvm::Triple &triple)
    : driver(driver), triple(triple) {}

Tool *ToolChain::PickTool(JobType jobType) const {
  switch (jobType) {
    case JobType::Compile:
      return stoneTool.get();
    case JobType::StaticLink:
    case JobType::DynamicLink:
      return linkTool.get();
    default:
      llvm_unreachable("Invalid tool type.");
  }
}

// TODO: Update for other scenarios like assemble
bool ToolChain::Build() {
  if (GetDriver().GetMode().CanCompile()) {
    assert(BuildStoneTool() && "Failed to build stone tool.");
    if (GetDriver().GetMode().IsCompileOnly()) {
      return true;
    }
  }
  // Build the link tools, ect.
  if (GetDriver().GetMode().CanLink()) {
    if (GetDriver().GetDriverOptions().useLLDLinker) {
      assert(BuildLLDLinkTool() && "Failed to build lld.");
    } else if (GetDriver().GetDriverOptions().useLDLinker) {
      assert(BuildLDLinkTool() && "Failed to build ld.");
    }
    if (GetDriver().GetMode().IsLinkOnly()) {
      return true;
    }
  }
  // Since we can do more than linking (emit-ir, ect.) first, build clang tool
  if (!BuildClangTool()) {
    // TODO: warn ... could not find clang, trying gcc
    assert(BuildGCCTool() && "Failed to build clang and gcc tools.");
  }
  return true;
}
