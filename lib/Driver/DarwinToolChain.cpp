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
bool DarwinToolChain::BuildDynamicLinkTool() { return true; }
bool DarwinToolChain::BuildStaticLinkTool() { return true; }
bool DarwinToolChain::BuildGCCTool() { return true; }
bool DarwinToolChain::BuildStoneTool() { return true; }

bool DarwinToolChain::Build() {
  if (GetDriver().GetMode().IsCompileOnly()) {
  }
  return true;
}
Tool *DarwinToolChain::GetTool(Mode mode) const { nullptr; }
Tool *DarwinToolChain::PickTool(const Job &job) const { return nullptr; }
