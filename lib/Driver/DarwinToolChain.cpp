#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

DarwinToolChain::DarwinToolChain(
    const Driver &driver, const llvm::Triple &triple,
    const llvm::Optional<llvm::Triple> &targetVariant)
    : ToolChain(driver, triple), targetVariant(targetVariant) {}

Tool *DarwinToolChain::BuildClangTool() const { return nullptr; }
Tool *DarwinToolChain::BuildAssembleTool() const { nullptr; }
Tool *DarwinToolChain::BuildDynamicLinkTool() const { nullptr; }
Tool *DarwinToolChain::BuildStaticLinkTool() const { nullptr; }
Tool *DarwinToolChain::BuildGCCTool() const { nullptr; }
Tool *DarwinToolChain::BuildStoneTool() const { nullptr; }

Tool *DarwinToolChain::GetTool(ModeKind kind) const { nullptr; }
