#include "stone/Syntax/DeclGroup.h"
#include "stone/Syntax/TreeContext.h"

#include <cassert>
#include <memory>

using namespace stone;

syn::DeclGroup *syn::DeclGroup::Create(TreeContext &tc, Decl **decls,
                                       unsigned numDecls) {

  // assert(numDecls > 1 && "Invalid DeclGroup");
  // unsigned totalSize = totalSizeToAlloc<Decl *>(numDecls);
  // void *mem = tc.Allocate(totalSize, alignof(DeclGroup));
  // new (mem) DeclGroup(numDecls, decls);
  // return static_cast<DeclGroup*>(mem);
  return nullptr;
}

syn::DeclGroup::DeclGroup(unsigned numDecls, Decl **decls)
    : NumDecls(numDecls) {

  // assert(numDecls > 0);
  // assert(decls);
  // std::uninitialized_copy(decls, decls + numDecls, getTrailingObjects<Decl
  // *>());
}
