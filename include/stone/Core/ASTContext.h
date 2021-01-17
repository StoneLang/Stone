#ifndef STONE_CORE_ASTCTX_H
#define STONE_CORE_ASTCTX_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/AlignOf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "stone/Core/ASTContextAlloc.h"
#include "stone/Core/Builtin.h"
#include "stone/Core/Context.h"
#include "stone/Core/Identifier.h"
#include "stone/Core/LangABI.h"
#include "stone/Core/LangOptions.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/Stats.h"
#include "stone/Core/Type.h"

namespace stone {
namespace syntax {

class BlockExpr;
class LangABI;
class Decl;
class ConstructorDecl;
class MethodDecl;
class RecordDecl;
class Expr;
class MangleCtx;
class Module;
class Stmt;
class Builtin;
class ASTContext;

class ASTContextStats final : public Stats {
  const ASTContext &ac;

 public:
  ASTContextStats(const ASTContext &ac) : ac(ac) {}
  void Print() const override;
};
class ASTContext final {
  friend ASTContextStats;
  ASTContextStats stats;

  /// The associated SourceManager object.
  SrcMgr &sm;

  /// The language options used to create the AST associated with
  ///  this ASTContext object.
  const stone::Context &ctx;

  /// The search path options
  const SearchPathOptions &searchPathOpts;

  Builtin builtin;
  /// The allocator used to create AST objects.
  /// AST objects are never destructed; rather, all memory associated with the
  /// AST objects will be released when the ASTContext itself is destroyed.
  mutable llvm::BumpPtrAllocator bumpAlloc;

  /// Table for all
  IdentifierTable identifiers;

  mutable llvm::SmallVector<Type *, 0> types;

 public:
  ASTContext(const stone::Context &ctx, const SearchPathOptions &pathOpts,
             SrcMgr &sm);
  ~ASTContext();

  ASTContext(const ASTContext &) = delete;
  ASTContext &operator=(const ASTContext &) = delete;

 public:
  //
  Identifier &GetIdentifier(llvm::StringRef name);
  //
  Builtin &GetBuiltin() const;
  //
  const stone::Context &GetContext() const { return ctx; }
  //
  LangABI *GetLangABI() const;
  //
  SrcMgr &GetSrcMgr() { return sm; }
  /// Retrieve the allocator for the given arena.
  llvm::BumpPtrAllocator &GetAllocator() const;

  ASTContextStats &GetStats() { return stats; }

 public:
  /// Return the total amount of physical memory allocated for representing
  /// AST nodes and type information.
  size_t GetSizeOfMemUsed() const;

  void *Allocate(size_t size, unsigned align = 8) const {
    return bumpAlloc.Allocate(size, align);
  }
  template <typename T>
  T *Allocate(size_t num = 1) const {
    return static_cast<T *>(Allocate(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *Ptr) const {}

 public:
};
}  // namespace syntax
}  // namespace stone
/// Placement new for using the ASTContext's allocator.
///
/// This placement form of operator new uses the ASTContext's allocator for
/// obtaining memory.
///
/// IMPORTANT: These are also declared in stone/AST/ASTContextAllocate.h!
/// Any changes here need to also be made there.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an ASTContext 'Context' in scope):
/// @code
/// // Default alignment (8)
/// IntegerLiteral *Ex = new (Context) IntegerLiteral(arguments);
/// // Specific alignment
/// IntegerLiteral *Ex2 = new (Context, 4) IntegerLiteral(arguments);
/// @endcode
/// Memory allocated through this placement new operator does not need to be
/// explicitly freed, as ASTContext will free all of this memory when it gets
/// destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the compiler.
/// @param C The ASTContext that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be nullptr.
inline void *operator new(size_t bytes, const stone::syntax::ASTContext &C,
                          size_t alignment /* = 8 */) {
  return C.Allocate(bytes, alignment);
}

/// Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the ASTContext throws in the object constructor.
inline void operator delete(void *Ptr, const stone::syntax::ASTContext &C,
                            size_t) {
  C.Deallocate(Ptr);
}

/// This placement form of operator new[] uses the ASTContext's allocator for
/// obtaining memory.
///
/// We intentionally avoid using a nothrow specification here so that the calls
/// to this operator will not perform a null check on the result -- the
/// underlying allocator never returns null pointers.
///
/// Usage looks like this (assuming there's an ASTContext 'Context' in scope):
/// @code
/// // Default alignment (8)
/// char *data = new (Context) char[10];
/// // Specific alignment
/// char *data = new (Context, 4) char[10];
/// @endcode
/// Memory allocated through this placement new[] operator does not need to be
/// explicitly freed, as ASTContext will free all of this memory when it gets
/// destroyed. Please note that you cannot use delete on the pointer.
///
/// @param Bytes The number of bytes to allocate. Calculated by the compiler.
/// @param C The ASTContext that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be nullptr.
inline void *operator new[](size_t bytes,
                            const stone::syntax::ASTContext &astCtx,
                            size_t alignment /* = 8 */) {
  return astCtx.Allocate(bytes, alignment);
}

/// Placement delete[] companion to the new[] above.
///
/// This operator is just a companion to the new[] above. There is no way of
/// invoking it directly; see the new[] operator for more details. This operator
/// is called implicitly by the compiler if a placement new[] expression using
/// the ASTContext throws in the object constructor.
inline void operator delete[](void *Ptr,
                              const stone::syntax::ASTContext &astCtx, size_t) {
  astCtx.Deallocate(Ptr);
}

#endif
