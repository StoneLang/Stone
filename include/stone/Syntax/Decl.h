#ifndef STONE_SYNTAX_DECL_H
#define STONE_SYNTAX_DECL_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/DeclBits.h"
#include "stone/Syntax/DeclName.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/TreeNode.h"

namespace stone {
namespace syn {

class Decl;
class BraceStmt;
class DeclContext;
class TreeContext;

class DeclStats final : public Stats {
  const Decl &declaration;

public:
  DeclStats(const Decl &declaration, Context &ctx)
      : Stats("ast-declaration stats:", ctx), declaration(declaration) {}
  void Print() override;
};

class alignas(8) Decl : public TreeNode {
public:
  enum Type {
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) LastDecl = Id,
#define DECL_RANGE(Id, FirstId, LastId)                                        \
  First##Id##Decl = FirstId, Last##Id##Decl = LastId,
#include "stone/Syntax/DeclType.def"
  };

private:
  friend DeclStats;
  Decl::Type ty;
  SrcLoc loc;
  DeclContext *dc;

protected:
  // TODO: This is how clang does it - swift

  /// Allocate memory for a deserialized declaration.
  ///
  /// This routine must be used to allocate memory for any declaration that is
  /// deserialized from a module file.
  ///
  /// \param size The size of the allocated object.
  /// \param astCtx The context in which we will allocate memory.
  /// \param declID The global ID of the deserialized declaration.
  /// \param extra The amount of extra space to allocate after the object.
  // void *operator new(std::size_t size, const TreeContext &tc, unsigned
  // declID,
  //                   std::size_t extra = 0);

  /// Allocate memory for a non-deserialized declaration.
  // void *operator new(std::size_t size, const TreeContext &astCtx,
  //                  DeclContext *parentDeclContext, std::size_t extra = 0);

public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;

  // Only allow allocation of Decls using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(size_t bytes, const TreeContext &tc,
                     unsigned alignment = alignof(Decl));

public:
  Decl() = delete;
  Decl(const Decl &) = delete;
  Decl(Decl &&) = delete;
  Decl &operator=(const Decl &) = delete;
  Decl &operator=(Decl &&) = delete;

  friend class DeclContext;

  struct MultipleDeclContext {
    DeclContext *semanticDeclContext;
    DeclContext *lexicalDeclContext;
  };

  llvm::PointerUnion<DeclContext *, MultipleDeclContext *> declCtx;

public:
  Decl::Type GetType() { return ty; }

  TreeContext &GetTreeContext() const LLVM_READONLY;

  /*
    const TreeContext &GetTreeContext() const {
      auto DC = context.dyn_cast<DeclContext *>();
      if (DC) {
        return DC->GetASTContext();
      }
      return *context.get<ASTContext *>();
    }

  */
protected:
  Decl(Decl::Type ty, DeclContext *dc, SrcLoc loc) : ty(ty), dc(dc), loc(loc) {}

public:
  template <typename DeclTy, typename AllocatorTy>
  static void *Make(AllocatorTy &allocatorTy, size_t baseSize) {
    return Make(allocatorTy, baseSize, false);
  }

  /// \param extraSpace The amount of extra space to allocate after the object
  /// -- generally for clang nodes.
  template <typename DeclTy, typename AllocatorTy>
  static void *Make(AllocatorTy &allocatorTy, size_t baseSize, bool extraSpace);
};

class DeclContext {
public:
  // TODO: Think about
  // enum class Type : unsigned { Module };

protected:
  /// This anonymous union stores the bits belonging to DeclContext and classes
  /// deriving from it. The goal is to use otherwise wasted
  /// space in DeclContext to store data belonging to derived classes.
  /// The space saved is especially significient when pointers are aligned
  /// to 8 bytes. In this case due to alignment requirements we have a
  /// little less than 8 bytes free in DeclContext which we can use.
  /// We check that none of the classes in this union is larger than
  /// 8 bytes with static_asserts in the ctor of DeclContext.
  union {
    DeclContextBits declContextBits;
    NominalTypeDeclBits nominalTypeDeclBits;
    // EnumDeclBitfields EnumDeclBits;
    // RecordDeclBitfields RecordDeclBits;
    FunctionDeclBits functionDeclBits;
    // ConstructorDeclBits constructorDeclBits;
    // LinkageSpecDeclBitfields LinkageSpecDeclBits;
    // BlockDeclBitfields BlockDeclBits;

    static_assert(sizeof(DeclContextBits) <= 8,
                  "DeclContextBitfields is larger than 8 bytes!");

    static_assert(sizeof(NominalTypeDeclBits) <= 8,
                  "TagDeclBitfields is larger than 8 bytes!");

    // static_assert(sizeof(EnumDeclBits) <= 8,
    //              "EnumDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(RecordDeclBits) <= 8,
    //              "RecordDeclBitfields is larger than 8 bytes!");
    static_assert(sizeof(FunctionDeclBits) <= 8,
                  "FunctionDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(ConstructorDeclBits) <= 8,
    //              "ConstructorDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(LinkageSpecDeclBits) <= 8,
    //              "LinkageSpecDeclBitfields is larger than 8 bytes!");
    // static_assert(sizeof(BlockDeclBitfields) <= 8,
    //              "BlockDeclBitfields is larger than 8 bytes!");
  };

protected:
  /// FirstDecl - The first declaration stored within this declaration
  /// context.
  mutable Decl *firstDecl = nullptr;

  /// LastDecl - The last declaration stored within this declaration
  /// context. FIXME: We could probably cache this value somewhere
  /// outside of the DeclContext, to reduce the size of DeclContext by
  /// another pointer.
  mutable Decl *lastDecl = nullptr;

  /// Build up a chain of declarations.
  ///
  /// \returns the first/last pair of declarations.
  static std::pair<Decl *, Decl *> BuildDeclChain(llvm::ArrayRef<Decl *> decls,
                                                  bool fieldsAlreadyLoaded);

public:
  DeclContext(Decl::Type ty, DeclContext *parent = nullptr);
};

class NamingDecl : public Decl {
  /// The name of this declaration, which is typically a normal
  /// identifier but may also be a special ty of name (C++
  /// constructor, etc.)
  DeclName name;

protected:
  NamingDecl(Decl::Type ty, DeclContext *dc, SrcLoc loc, DeclName name)
      : Decl(ty, dc, loc), name(name) {}

public:
  /// Get the identifier that names this declaration, if there is one.
  ///
  /// This will return NULL if this declaration has no name (e.g., for
  /// an unnamed class) or if the name is a special name such ast a C++
  /// constructor.
  Identifier *GetIdentifier() const { return name.GetAsIdentifier(); }

  /// Get the name of identifier for this declaration as a StringRef.
  ///
  /// This requires that the declaration have a name and that it be a simple
  /// identifier.
  llvm::StringRef GetName() const {
    // TODO: assert(name.IsIdentifier() && "Name is not a simple identifier");
    return GetIdentifier() ? GetIdentifier()->GetName() : "";
  }
};

class TypeDecl : public NamingDecl {

  friend class TreeContext;
  /// This indicates the Type object that represents
  /// this TypeDecl.  It is a cache maintained by
  /// ASTContext::getTypedefType, ASTContext::getTagDeclType, and
  /// ASTContext::getTemplateTypeParmType, and TemplateTypeParmDecl.
  // mutable const Type *TypeForDecl = nullptr;

  /// The start of the source range for this declaration.
  SrcLoc startLoc;

protected:
  TypeDecl(Decl::Type ty, DeclContext *dc, SrcLoc loc, Identifier *name,
           SrcLoc startLocation = SrcLoc())
      : NamingDecl(ty, dc, loc, name), startLoc(startLocation) {}
};

// TODO: May use this instead of using NamingDecl
class ValueDecl : public NamingDecl {
public:
};

class LabelDecl : public NamingDecl {
public:
};

class SpaceDecl : public NamingDecl {
public:
  SpaceDecl(DeclContext *dc, SrcLoc loc, DeclName name)
      : NamingDecl(Decl::Type::Space, dc, loc, name) {}
};

class DeclaratorDecl : public ValueDecl {
public:
};

class FunctionDecl : public DeclaratorDecl, public DeclContext {
public:
};

class FunDecl : public FunctionDecl {
public:
public:
  static FunDecl *Create(TreeContext *tc, DeclContext *dc);
};

class ConstructorInitializer final {
public:
};

class ConstructorDecl : public FunctionDecl {
public:
};

class DestructorDecl : public FunctionDecl {
public:
};

} // namespace syn
} // namespace stone
#endif
