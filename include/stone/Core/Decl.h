#ifndef STONE_CORE_DECL_H
#define STONE_CORE_DECL_H

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
#include "stone/Core/ASTNode.h"
#include "stone/Core/DeclBits.h"
#include "stone/Core/DeclKind.h"
#include "stone/Core/DeclName.h"
#include "stone/Core/Identifier.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/Object.h"
#include "stone/Core/SrcLoc.h"

namespace stone {
namespace syntax {

class Decl;
class BraceStmt;
class DeclContext;
class ASTContext;

class DeclStats final : public Stats {
  const Decl &declaration;

 public:
  DeclStats(const Decl &declaration) : declaration(declaration) {}
  void Print() const override;
};

class alignas(8) Decl : public ASTNode /*TODO: Object */ {
  friend DeclStats;
  decl::Kind kind;
  SrcLoc loc;
  DeclContext *dc;

 public:
  /*
    enum class Kind : unsigned {
  #define DECL(Id, Parent) Id,
  #define LAST_DECL(Id) LastDecl = Id,
  #define DECL_RANGE(Id, FirstId, LastId) \ First##Id##Decl = FirstId,
  Last##Id##Decl = LastId, #include "stone/Core/DeclKind.def"
    };
  */

 protected:
  /// Allocate memory for a deserialized declaration.
  ///
  /// This routine must be used to allocate memory for any declaration that is
  /// deserialized from a module file.
  ///
  /// \param size The size of the allocated object.
  /// \param astCtx The context in which we will allocate memory.
  /// \param declID The global ID of the deserialized declaration.
  /// \param extra The amount of extra space to allocate after the object.
  void *operator new(std::size_t size, const ASTContext &astCtx,
                     unsigned declID, std::size_t extra = 0);

  /// Allocate memory for a non-deserialized declaration.
  void *operator new(std::size_t size, const ASTContext &astCtx,
                     DeclContext *parentDeclContext, std::size_t extra = 0);

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
  decl::Kind GetKind() { return kind; }

 protected:
  Decl(decl::Kind kind, DeclContext *dc, SrcLoc loc)
      : kind(kind), dc(dc), loc(loc) {}
};

class DeclContext {
 public:
  // TODO: Think about
  enum class Kind : unsigned { Module };

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
};

class NamingDecl : public Decl {
  /// The name of this declaration, which is typically a normal
  /// identifier but may also be a special kind of name (C++
  /// constructor, etc.)
  DeclName name;

 protected:
  NamingDecl(decl::Kind kind, DeclContext *dc, SrcLoc loc, DeclName name)
      : Decl(kind, dc, loc), name(name) {}

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
 public:
};

class ValueDecl : public NamingDecl {
 public:
};

class SpaceDecl : public NamingDecl {
 public:
  SpaceDecl(DeclContext *dc, SrcLoc loc, DeclName name)
      : NamingDecl(decl::Kind::Space, dc, loc, name) {}
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
  static FunDecl *Create();
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
}  // namespace syntax
}  // namespace stone
#endif
