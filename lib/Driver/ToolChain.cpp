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
StoneTool::~StoneTool() {}

ClangTool::ClangTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Clang, toolChain) {
  toolOpts.canEmitIR = true;
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}

ClangTool::~ClangTool() {}

GCCTool::GCCTool(llvm::StringRef fullName, llvm::StringRef shortName,
                 const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::GCC, toolChain) {
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}
GCCTool::~GCCTool() {}

LinkTool::LinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                   ToolType toolType, const ToolChain &toolChain,
                   LinkType linkType)
    : Tool(fullName, shortName, toolType, toolChain), linkType(linkType) {
  toolOpts.canLink = true;
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
AssembleTool::~AssembleTool() {}

ToolChain::ToolChain(const Driver &driver, const llvm::Triple &triple)
    : driver(driver), triple(triple) {}

std::unique_ptr<Job> ToolChain::CreateJob(/*const JobAction &JA, Compilation &C,
                                    SmallVectorImpl<const Job *> &&inputs,
                                    ArrayRef<const Action *> inputActions,
                                    std::unique_ptr<CommandOutput> output,
																		const OutputInfo &OI*/) const {
  return nullptr;
}

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
