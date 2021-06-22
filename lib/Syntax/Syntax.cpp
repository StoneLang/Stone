#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(TreeContext &tc) : tc(tc) {}

Syntax::~Syntax() {}

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
