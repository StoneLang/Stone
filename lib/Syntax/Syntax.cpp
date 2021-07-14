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

template <typename DeclTy, typename AllocatorTy>
void *Syntax::AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                              bool extraSace) {

  static_assert(alignof(DeclTy) >= sizeof(void *),
                "A pointer must fit in the alignment of the DeclTy!");

  size_t size = baseSize;
  if (extraSace) {
    size += alignof(DeclTy);
  }
  void *mem = allocatorTy.Allocate(size, alignof(DeclTy));
  if (extraSace)
    mem = reinterpret_cast<char *>(mem) + alignof(DeclTy);
  return mem;
}

Module *Syntax::CreateModuleDecl(Identifier &name, bool isMainModule) {

  void *moduleDeclPtr = Syntax::AllocateDeclMem<Module>(GetTreeContext(), sizeof(Module));

  auto moduleDecl = new (GetTreeContext()) syn::Module(name, GetTreeContext());
  return moduleDecl;
}

FunDecl *Syntax::CreateFunDecl() {
  size_t size =
      sizeof(FunDecl); // + (HasImplicitThisDecl ? sizeof(ParamDecl *) : 0);

  void *funDeclPtr = Syntax::AllocateDeclMem<FunDecl>(GetTreeContext(), size);
}
