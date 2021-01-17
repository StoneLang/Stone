#ifndef STONE_CORE_LIST_H
#define STONE_CORE_LIST_H

#include <memory>
#include <string>
#include <utility>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "stone/Core/LLVM.h"

namespace stone {

/// JobList - A sequence of jobs to perform.
template <typename T>
class List {
 public:
  using list_type = llvm::SmallVector<std::unique_ptr<T>, 4>;
  using size_type = typename list_type::size_type;
  using iterator = llvm::pointee_iterator<typename list_type::iterator>;
  using const_iterator =
      llvm::pointee_iterator<typename list_type::const_iterator>;

 private:
  list_type entries;

 public:
  /// Add a job to the list (taking ownership).
  void Add(std::unique_ptr<T> entry) { entries.push_back(std::move(entry)); }
  /// Clear the list
  void Clear() { entries.clear(); }

  const list_type &GetEntries() const { return entries; }

  bool empty() const { return entries.empty(); }
  size_type size() const { return entries.size(); }
  iterator begin() { return entries.begin(); }
  const_iterator begin() const { return entries.begin(); }
  iterator end() { return entries.end(); }
  const_iterator end() const { return entries.end(); }
};

}  // namespace stone
#endif
