#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

Tool::Tool(llvm::StringRef fullName, llvm::StringRef shortName, ToolType ty,
           const ToolChain &toolChain)
    : fullName(fullName), shortName(shortName), toolChain(toolChain) {}

Tool::~Tool() {}

ClangTool::ClangTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Clang, toolChain) {
  toolOpts.canEmitIR = true;
  toolOpts.canAssemble = true;
  toolOpts.canLink = true;
}

ClangTool::~ClangTool() {}

StoneTool::StoneTool(llvm::StringRef fullName, llvm::StringRef shortName,
                     const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Stone, toolChain) {
  toolOpts.canEmitIR = true;
}
StoneTool::~StoneTool() {}

GCCTool::GCCTool(llvm::StringRef fullName, llvm::StringRef shortName,
                 const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::GCC, toolChain) {}
GCCTool::~GCCTool() {}

DynamicLinkTool::DynamicLinkTool(llvm::StringRef fullName,
                                 llvm::StringRef shortName,
                                 const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::DynamicLink, toolChain) {}
DynamicLinkTool::~DynamicLinkTool() {}

StaticLinkTool::StaticLinkTool(llvm::StringRef fullName,
                               llvm::StringRef shortName,
                               const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::StaticLink, toolChain) {}

StaticLinkTool::~StaticLinkTool() {}

AssembleTool::AssembleTool(llvm::StringRef fullName, llvm::StringRef shortName,
                           const ToolChain &toolChain)
    : Tool(fullName, shortName, ToolType::Assemble, toolChain) {}
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

Tool *ToolChain::PickTool(const Job &job) const {
  switch (job.GetType()) {
    case JobType::DynamicLink:
      return nullptr;
    default:
      return nullptr;
  }
  return nullptr;
}
