#ifndef STONE_COREDIAGNOSTIC_H
#define STONE_COREDIAGNOSTIC_H

#include "stone/Utils/DiagnosticEngine.h"
#include "stone/Utils/LLVM.h"

namespace stone {
template <typename... argTypes> struct Diag;

namespace detail {
// These templates are used to help extract the type arguments of the
// DIAG/ERROR/WARNING/NOTE/REMARK/FIXIT macros.
template <typename T> struct DiagWithArguments;

template <typename... argTypes> struct DiagWithArguments<void(argTypes...)> {
  typedef Diag<argTypes...> type;
};

} // end namespace detail

enum class StaticSpellingKind : uint8_t;

namespace diag {
enum class RequirementKind : uint8_t;
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "CoreDiagnostic.def"

} // end namespace diag
} // end namespace stone

#endif
