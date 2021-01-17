#ifndef STONE_CORE_DECLBITS_H
#define STONE_CORE_DECLBITS_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
namespace syntax {

class NominalTypeDecl;
class DeclContext;
class FunctionDecl;

/// Stores the bits used by DeclContext.
/// If modified NumDeclContextBit, the ctor of DeclContext and the accessor
/// methods in DeclContext should be updated appropriately.
class DeclContextBits {
  friend class DeclContext;
  /// DeclKind - This indicates which class this is.
  uint64_t DeclKind : 7;

  /// Whether this declaration context also has some external
  /// storage that contains additional declarations that are lexically
  /// part of this context.
  mutable uint64_t ExternalLexicalStorage : 1;

  /// Whether this declaration context also has some external
  /// storage that contains additional declarations that are visible
  /// in this context.
  mutable uint64_t ExternalVisibleStorage : 1;

  /// Whether this declaration context has had externally visible
  /// storage added since the last lookup. In this case, \c LookupPtr's
  /// invariant may not hold and needs to be fixed before we perform
  /// another lookup.
  mutable uint64_t NeedToReconcileExternalVisibleStorage : 1;

  /// If \c true, this context may have local lexical declarations
  /// that are missing from the lookup table.
  mutable uint64_t HasLazyLocalLexicalLookups : 1;

  /// If \c true, the external source may have lexical declarations
  /// that are missing from the lookup table.
  mutable uint64_t HasLazyExternalLexicalLookups : 1;

  /// If \c true, lookups should only return identifier from
  /// DeclContext scope (for example TranslationUnit). Used in
  /// LookupQualifiedName()
  mutable uint64_t UseQualifiedLookup : 1;
};

/// Number of bits in DeclContextBitfields.
enum { NumDeclContextBits = 13 };

/// Stores the bits used by TagDecl.
/// If modified NumTagDeclBits and the accessor
/// methods in TagDecl should be updated appropriately.
class NominalTypeDeclBits {
  friend class NominalTypeDecl;
  /// For the bits in DeclContextBitfields
  uint64_t : NumDeclContextBits;

  /// The TagKind enum.
  uint64_t TagDeclKind : 3;

  /// True if this is a definition ("struct foo {};"), false if it is a
  /// declaration ("struct foo;").  It is not considered a definition
  /// until the definition has been fully processed.
  uint64_t IsCompleteDefinition : 1;

  /// True if this is currently being defined.
  uint64_t IsBeingDefined : 1;

  /// True if this tag declaration is "embedded" (i.e., defined or declared
  /// for the very first time) in the syntax of a declarator.
  uint64_t IsEmbeddedInDeclarator : 1;

  /// True if this tag is free standing, e.g. "struct foo;".
  uint64_t IsFreeStanding : 1;

  /// Indicates whether it is possible for declarations of this kind
  /// to have an out-of-date definition.
  ///
  /// This option is only enabled when modules are enabled.
  uint64_t MayHaveOutOfDateDef : 1;

  /// Has the full definition of this type been required by a use somewhere in
  /// the TU.
  uint64_t IsCompleteDefinitionRequired : 1;
};

/// Number of non-inherited bits in TagDeclBitfields.
enum { NumNominalTypeDeclBits = 9 };

/// Stores the bits used by FunctionDecl.
/// If modified NumFunctionDeclBits and the accessor
/// methods in FunctionDecl and CXXDeductionGuideDecl
/// (for IsCopyDeductionCandidate) should be updated appropriately.
class FunctionDeclBits final {
  friend class FunctionDecl;
  /// For IsCopyDeductionCandidate
  // TODO: friend class DeductionGuideDecl;
  /// For the bits in DeclContextBitfields.
  uint64_t : NumDeclContextBits;

  uint64_t SClass : 3;
  uint64_t IsInline : 1;
  uint64_t IsInlineSpecified : 1;

  uint64_t IsVirtualAsWritten : 1;
  uint64_t IsPure : 1;
  uint64_t HasInheritedPrototype : 1;
  uint64_t HasWrittenPrototype : 1;
  uint64_t IsDeleted : 1;

  /// Used by CXXMethodDecl
  uint64_t IsTrivial : 1;

  /// This flag indicates whether this function is trivial for the purpose of
  /// calls. This is meaningful only when this function is a copy/move
  /// constructor or a destructor.
  uint64_t IsTrivialForCall : 1;

  uint64_t IsDefaulted : 1;
  uint64_t IsExplicitlyDefaulted : 1;
  uint64_t HasDefaultedFunctionInfo : 1;
  uint64_t HasImplicitReturnZero : 1;
  uint64_t IsLateTemplateParsed : 1;

  /// Kind of contexpr specifier as defined by ConstexprSpecKind.
  uint64_t ConstexprKind : 2;
  uint64_t InstantiationIsPending : 1;

  /// Indicates if the function was a definition
  /// but its body was skipped.
  uint64_t HasSkippedBody : 1;

  /// Indicates if the function declaration will
  /// have a body, once we're done parsing it.
  uint64_t WillHaveBody : 1;

  /// Indicates that this function is a multiversioned
  /// function using attribute 'target'.
  uint64_t IsMultiVersion : 1;

  /// [C++17] Only used by CXXDeductionGuideDecl. Indicates that
  /// the Deduction Guide is the implicitly generated 'copy
  /// deduction candidate' (is used during overload resolution).
  uint64_t IsCopyDeductionCandidate : 1;

  /// Store the ODRHash after first calculation.
  uint64_t HasODRHash : 1;

  /// Indicates if the function uses Floating Point Constrained Intrinsics
  uint64_t UsesFPIntrin : 1;
};

/// Number of non-inherited bits in FunctionDeclBitfields.
enum { NumFunctionDeclBits = 27 };
}  // namespace syntax
}  // namespace stone

#endif
