#ifndef STONE_SYNTAX_DECLGROUP_H
#define STONE_SYNTAX_DECLGROUP_H

#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstdint>

namespace stone {
namespace syn {

class Decl;
class TreeContext;

class DeclGroup final : private llvm::TrailingObjects<DeclGroup, Decl *> {
  // FIXME: Include a TypeSpecifier object.
  unsigned NumDecls = 0;

private:
  DeclGroup() = default;
  DeclGroup(unsigned numdecls, Decl **decls);

public:
  friend TrailingObjects;

  static DeclGroup *Create(TreeContext &tc, Decl **decls, unsigned numDecls);

  unsigned size() const { return NumDecls; }

  Decl *&operator[](unsigned i) {
    assert(i < NumDecls && "Out-of-bounds access.");
    return getTrailingObjects<Decl *>()[i];
  }

  Decl *const &operator[](unsigned i) const {
    assert(i < NumDecls && "Out-of-bounds access.");
    return getTrailingObjects<Decl *>()[i];
  }
};

class DeclGroupRef {
  // Note this is not a PointerIntPair because we need the address of the
  // non-group case to be valid as a Decl** for iteration.
  enum Kind { SingleDeclKind = 0x0, DeclGroupKind = 0x1, Mask = 0x1 };

  Decl *D = nullptr;

  Kind getKind() const { return (Kind)(reinterpret_cast<uintptr_t>(D) & Mask); }

public:
  DeclGroupRef() = default;
  explicit DeclGroupRef(Decl *d) : D(d) {}
  explicit DeclGroupRef(DeclGroup *dg)
      : D((Decl *)(reinterpret_cast<uintptr_t>(dg) | DeclGroupKind)) {}

  static DeclGroupRef Create(TreeContext &C, Decl **Decls, unsigned NumDecls) {
    if (NumDecls == 0)
      return DeclGroupRef();
    if (NumDecls == 1)
      return DeclGroupRef(Decls[0]);
    return DeclGroupRef(DeclGroup::Create(C, Decls, NumDecls));
  }

  using iterator = Decl **;
  using const_iterator = Decl *const *;

  bool isNull() const { return D == nullptr; }
  bool isSingleDecl() const { return getKind() == SingleDeclKind; }
  bool isDeclGroup() const { return getKind() == DeclGroupKind; }

  Decl *getSingleDecl() {
    assert(isSingleDecl() && "Isn't a single decl");
    return D;
  }
  const Decl *getSingleDecl() const {
    return const_cast<DeclGroupRef *>(this)->getSingleDecl();
  }

  DeclGroup &getDeclGroup() {
    assert(isDeclGroup() && "Isn't a declgroup");
    return *((DeclGroup *)(reinterpret_cast<uintptr_t>(D) & ~Mask));
  }
  const DeclGroup &getDeclGroup() const {
    return const_cast<DeclGroupRef *>(this)->getDeclGroup();
  }

  iterator begin() {
    if (isSingleDecl())
      return D ? &D : nullptr;
    return &getDeclGroup()[0];
  }

  iterator end() {
    if (isSingleDecl())
      return D ? &D + 1 : nullptr;
    DeclGroup &G = getDeclGroup();
    return &G[0] + G.size();
  }

  const_iterator begin() const {
    if (isSingleDecl())
      return D ? &D : nullptr;
    return &getDeclGroup()[0];
  }

  const_iterator end() const {
    if (isSingleDecl())
      return D ? &D + 1 : nullptr;
    const DeclGroup &G = getDeclGroup();
    return &G[0] + G.size();
  }

  void *getAsOpaquePtr() const { return D; }
  static DeclGroupRef getFromOpaquePtr(void *Ptr) {
    DeclGroupRef X;
    X.D = static_cast<Decl *>(Ptr);
    return X;
  }
};
} // namespace syn
} // namespace stone

namespace llvm {

// DeclGroupRef is "like a pointer", implement PointerLikeTypeTraits.
template <typename T> struct PointerLikeTypeTraits;
template <> struct PointerLikeTypeTraits<stone::syn::DeclGroupRef> {
  static inline void *getAsVoidPointer(stone::syn::DeclGroupRef P) {
    return P.getAsOpaquePtr();
  }

  static inline stone::syn::DeclGroupRef getFromVoidPointer(void *P) {
    return stone::syn::DeclGroupRef::getFromOpaquePtr(P);
  }

  static constexpr int NumLowBitsAvailable = 0;
};

} // namespace llvm

#endif
