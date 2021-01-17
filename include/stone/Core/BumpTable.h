#ifndef STONE_CORE_BUMPTABLE_H
#define STONE_CORE_BUMPTABLE_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"

namespace stone {
template <typename T>
class BumpTable {
  llvm::StringMap<T *, llvm::BumpPtrAllocator> entries;

 public:
  llvm::BumpPtrAllocator &GetAllocator() { return entries.getAllocator(); }
  /// Return the identifier token info for the specified named
  /// identifier.
  T &Get(llvm::StringRef name) {
    auto &entry = *entries.insert(std::make_pair(name, nullptr)).first;
    T *&second = entry.second;
    if (second) {
      return *second;
    }
    // Lookups failed, make a new Identifier.
    void *mem = GetAllocator().template Allocate<T>();
    second = new (mem) T();
    return *second;
  }
};
}  // namespace stone

#endif
