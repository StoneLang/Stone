#include "stone/Driver/ToolChain.h"

#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::driver;

Tool::Tool(llvm::StringRef fullName, llvm::StringRef shortName,
           ToolType toolType, const ToolChain &toolChain)
    : fullName(fullName), shortName(shortName), toolType(toolType),
      toolChain(toolChain) {}

Tool::~Tool() {}

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
