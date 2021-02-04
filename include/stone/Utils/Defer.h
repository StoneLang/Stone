#ifndef STONE_DEFER_H
#define STONE_DEFER_H

#include "llvm/ADT/ScopeExit.h"

namespace stone {
namespace detail {
struct DeferTask {};
template <typename F>
auto operator+(DeferTask, F &&fn)
    -> decltype(llvm::make_scope_exit(std::forward<F>(fn))) {
  return llvm::make_scope_exit(std::forward<F>(fn));
}
} // namespace detail
} // end namespace stone

#define DEFER_CONCAT_IMPL(x, y) x##y
#define DEFER_MACRO_CONCAT(x, y) DEFER_CONCAT_IMPL(x, y)

/// This macro is used to register a function / lambda to be run on exit from a
/// scope.  Its typical use looks like:
///
///   STONE_DEFER {
///     stuff
///   };
///
#define STONE_DEFER                                                            \
  auto DEFER_MACRO_CONCAT(defer_func, __COUNTER__) =                           \
      ::stone::detail::DeferTask() + [&]()

#endif // STONE_BASIC_DEFER_H
