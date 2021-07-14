#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(TreeContext &tc) : tc(tc) {}
Syntax::~Syntax() {}

template <std::size_t Len>
static bool IsNamed(const NamingDecl *namingDecl, const char (&str)[Len]) {
  Identifier *identifier = namingDecl->GetIdentifier();
  return identifier && identifier->isStr(str);
}

Module *Syntax::CreateModuleDecl(Identifier &name, bool isMainModule) {
  auto moduleDecl = new (GetTreeContext()) syn::Module(name, GetTreeContext());
  return moduleDecl;
}

FunDecl *Syntax::CreateFunDecl() { return nullptr; }
