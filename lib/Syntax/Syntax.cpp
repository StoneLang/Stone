#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(TreeContext &tc)
    : tc(tc), funBuilder(*this), structBuilder(*this) {}

Syntax::~Syntax() {}

DeclFactory::~DeclFactory() {}
FunDeclFactory::~FunDeclFactory() {}

// Going this route for the time being
FunDeclFactory &Syntax::GetFunDeclFactory() {
  if (funDeclFactory) {
    return *funDeclFactory.get();
  }
  funDeclFactory = llvm::make_unique<FunDeclFactory>(*this);
  return *funDeclFactory.get();
}

StructDeclFactory::~StructDeclFactory() {}

StructDeclFactory &Syntax::GetStructDeclFactory() {
  if (structDeclFactory) {
    return *structDeclFactory.get();
  }
  structDeclFactory = llvm::make_unique<StructDeclFactory>(*this);
  return *structDeclFactory.get();
}
ModuleDeclFactory &Syntax::GetModuleDeclFactory() {
  if (moduleDeclFactory) {
    return *moduleDeclFactory.get();
  }
  moduleDeclFactory = llvm::make_unique<ModuleDeclFactory>(*this);
  return *moduleDeclFactory.get();
}
ModuleDeclFactory::~ModuleDeclFactory() {}

Module *Syntax::CreateModuleDecl(Identifier &name, bool isMainModule) {
  auto moduleDecl = new (GetTreeContext()) syn::Module(name, GetTreeContext());
  return moduleDecl;
}

FunDecl *Syntax::CreateFunDecl() { return nullptr; }
