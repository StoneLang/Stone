#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Type.h"

namespace stone {
namespace syn {
class Decl;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class InflightDiagnostic;

// Determines whether the low bit of the result pointer for the
// given UID is always zero. If so, SyntaxResult will use that bit
// for it's "invalid" flag.
template <class Ptr> struct IsResultPtrLowBitFree {
  static const bool value = false;
};

/// SyntaxResult - This structure is used while parsing/acting on
/// expressions, stmts, etc.  It encapsulates both the object returned by
/// the action, plus a sense of whether or not it is valid.
/// When CompressInvalid is true, the "invalid" flag will be
/// stored in the low bit of the Val pointer.
template <class PtrTy,
          bool CompressInvalid = IsResultPtrLowBitFree<PtrTy>::value>
class SyntaxResult {
  PtrTy Val;
  bool Invalid;

public:
  SyntaxResult(bool Invalid = false) : Val(PtrTy()), Invalid(Invalid) {}
  SyntaxResult(PtrTy val) : Val(val), Invalid(false) {}
  SyntaxResult(const InflightDiagnostic &) : Val(PtrTy()), Invalid(true) {}

  // These two overloads prevent void* -> bool conversions.
  SyntaxResult(const void *) = delete;
  SyntaxResult(volatile void *) = delete;

  bool isInvalid() const { return Invalid; }
  bool isUsable() const { return !Invalid && Val; }
  bool isUnset() const { return !Invalid && !Val; }

  PtrTy get() const { return Val; }
  template <typename T> T *getAs() { return static_cast<T *>(get()); }

  void set(PtrTy V) { Val = V; }

  const SyntaxResult &operator=(PtrTy RHS) {
    Val = RHS;
    Invalid = false;
    return *this;
  }
};
// This SyntaxResult partial specialization places the "invalid"
// flag into the low bit of the pointer.
template <typename PtrTy> class SyntaxResult<PtrTy, true> {
  // A pointer whose low bit is 1 if this result is invalid, 0
  // otherwise.
  uintptr_t PtrWithInvalid;

  using PtrTraits = llvm::PointerLikeTypeTraits<PtrTy>;

public:
  SyntaxResult(bool Invalid = false)
      : PtrWithInvalid(static_cast<uintptr_t>(Invalid)) {}

  SyntaxResult(PtrTy V) {
    void *VP = PtrTraits::getAsVoidPointer(V);
    PtrWithInvalid = reinterpret_cast<uintptr_t>(VP);
    assert((PtrWithInvalid & 0x01) == 0 && "Badly aligned pointer");
  }

  SyntaxResult(const InflightDiagnostic &) : PtrWithInvalid(0x01) {}

  // These two overloads prevent void* -> bool conversions.
  SyntaxResult(const void *) = delete;
  SyntaxResult(volatile void *) = delete;

  bool isInvalid() const { return PtrWithInvalid & 0x01; }
  bool isUsable() const { return PtrWithInvalid > 0x01; }
  bool isUnset() const { return PtrWithInvalid == 0; }

  PtrTy get() const {
    void *VP = reinterpret_cast<void *>(PtrWithInvalid & ~0x01);
    return PtrTraits::getFromVoidPointer(VP);
  }

  template <typename T> T *getAs() { return static_cast<T *>(get()); }

  void set(PtrTy V) {
    void *VP = PtrTraits::getAsVoidPointer(V);
    PtrWithInvalid = reinterpret_cast<uintptr_t>(VP);
    assert((PtrWithInvalid & 0x01) == 0 && "Badly aligned pointer");
  }

  const SyntaxResult &operator=(PtrTy RHS) {
    void *VP = PtrTraits::getAsVoidPointer(RHS);
    PtrWithInvalid = reinterpret_cast<uintptr_t>(VP);
    assert((PtrWithInvalid & 0x01) == 0 && "Badly aligned pointer");
    return *this;
  }

  // For types where we can fit a flag in with the pointer, provide
  // conversions to/from pointer type.
  static SyntaxResult getFromOpaquePointer(void *P) {
    SyntaxResult Result;
    Result.PtrWithInvalid = (uintptr_t)P;
    return Result;
  }
  void *getAsOpaquePointer() const { return (void *)PtrWithInvalid; }
};

/// An opaque type for threading parsed type information through the
/// parser.
// using ParsedType = OpaquePtr<QualType>;
// using UnionParsedType = UnionOpaquePtr<QualType>;

// We can re-use the low bit of expression, statement, base, and
// member-initializer pointers for the "invalid" flag of
// SyntaxResult.
template <> struct IsResultPtrLowBitFree<Expr *> {
  static const bool value = true;
};
template <> struct IsResultPtrLowBitFree<Stmt *> {
  static const bool value = true;
};

// template<> struct IsResultPtrLowBitFree<BaseSpecifier*> {
//  static const bool value = true;
//};
// template<> struct IsResultPtrLowBitFree<CtorInitializer*> {
//  static const bool value = true;
//};

using ExprResult = SyntaxResult<Expr *>;
using StmtResult = SyntaxResult<Stmt *>;
// using TypeResult = SyntaxResult<ParsedType>;
// using BaseResult = SyntaxResult<BaseSpecifier *>;
// using MemInitResult = SyntaxResult<CtorInitializer *>;

using DeclResult = SyntaxResult<Decl *>;
// using ParsedTemplateTy = OpaquePtr<TemplateName>;
// using UnionParsedTemplateTy = UnionOpaquePtr<TemplateName>;

using MultiExprArg = MutableArrayRef<Expr *>;
using MultiStmtArg = MutableArrayRef<Stmt *>;
// using TreeTemplateArgsPtr = MutableArrayRef<ParsedTemplateArgument>;
// using MultiTypeArg = MutableArrayRef<ParsedType>;
// sing MultiTemplateParamsArg = MutableArrayRef<TemplateParameterList *>;

inline ExprResult ExprError() { return ExprResult(true); }
inline StmtResult StmtError() { return StmtResult(true); }
// inline TypeResult TypeError() { return TypeResult(true); }

inline ExprResult ExprError(const InflightDiagnostic &) { return ExprError(); }
inline StmtResult StmtError(const InflightDiagnostic &) { return StmtError(); }

inline ExprResult ExprEmpty() { return ExprResult(false); }
inline StmtResult StmtEmpty() { return StmtResult(false); }

inline Expr *AssertSuccess(ExprResult R) {
  assert(!R.isInvalid() && "operation was asserted to never fail!");
  return R.get();
}

inline Stmt *AssertSuccess(StmtResult R) {
  assert(!R.isInvalid() && "operation was asserted to never fail!");
  return R.get();
}

class Syntax final {
public:
  static FunDecl *CreateFunDecl();
  static StructDecl *CreateStructDecl();

public:
  static IfStmt *CreateIfStmt();
};
} // namespace syn
} // namespace stone
#endif
