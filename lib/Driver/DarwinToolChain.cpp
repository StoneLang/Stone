#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

DarwinToolChain::DarwinToolChain(
    const Driver &driver, const llvm::Triple &triple,
    const llvm::Optional<llvm::Triple> &targetVariant)
    : ToolChain(driver, triple), targetVariant(targetVariant) {}

bool DarwinToolChain::BuildClangTool() { return true; }
bool DarwinToolChain::BuildAssembleTool() { return true; }
bool DarwinToolChain::BuildLLDLinkTool() { return true; }
bool DarwinToolChain::BuildLDLinkTool() { return true; }
bool DarwinToolChain::BuildGCCTool() { 
 //gccTool = llvm::make_unique<GCCTool>("test", "test", *this);
  return true;
}

bool DarwinToolChain::BuildStoneTool() {
  stoneTool = llvm::make_unique<StoneTool>("test", "test", *this);
  return true;
}

Tool *DarwinToolChain::PickTool(JobType jobType) const {
  return ToolChain::PickTool(jobType);
}
