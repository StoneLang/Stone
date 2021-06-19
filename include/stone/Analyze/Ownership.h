//===- Ownership.h - Parser ownership helpers -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file contains classes for managing ownership of Stmt and Expr nodes.
//
//===----------------------------------------------------------------------===//

#ifndef STONE_ANALYZE_OWNERSHIP_H
#define STONE_ANALYZE_OWNERSHIP_H

#include "stone/Basic/LLVM.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

//===----------------------------------------------------------------------===//
// OpaquePtr
//===----------------------------------------------------------------------===//

// TODO: move to basic
namespace stone {
/// Wrapper for void* pointer.
/// \tparam PtrTy Either a pointer type like 'T*' or a type that behaves like
///               a pointer.
///
/// This is a very simple POD type that wraps a pointer that the Parser
/// doesn't know about but that Sema or another client does.  The PtrTy
/// template argument is used to make sure that "Decl" pointers are not
/// compatible with "Type" pointers for example.
template <class PtrTy> class OpaquePtr {
  void *Ptr = nullptr;

  explicit OpaquePtr(void *Ptr) : Ptr(Ptr) {}

  using Traits = llvm::PointerLikeTypeTraits<PtrTy>;

public:
  OpaquePtr(std::nullptr_t = nullptr) {}

  static OpaquePtr make(PtrTy P) {
    OpaquePtr OP;
    OP.set(P);
    return OP;
  }

  /// Returns plain pointer to the entity pointed by this wrapper.
  /// \tparam PointeeT Type of pointed entity.
  ///
  /// It is identical to getPtrAs<PointeeT*>.
  template <typename PointeeT> PointeeT *getPtrTo() const { return get(); }

  /// Returns pointer converted to the specified type.
  /// \tparam PtrT Result pointer type.  There must be implicit conversion
  ///              from PtrTy to PtrT.
  ///
  /// In contrast to getPtrTo, this method allows the return type to be
  /// a smart pointer.
  template <typename PtrT> PtrT getPtrAs() const { return get(); }

  PtrTy get() const { return Traits::getFromVoidPointer(Ptr); }

  void set(PtrTy P) { Ptr = Traits::getAsVoidPointer(P); }

  explicit operator bool() const { return Ptr != nullptr; }

  void *getAsOpaquePtr() const { return Ptr; }
  static OpaquePtr getFromOpaquePtr(void *P) { return OpaquePtr(P); }
};

/// UnionOpaquePtr - A version of OpaquePtr suitable for membership
/// in a union.
template <class T> struct UnionOpaquePtr {
  void *Ptr;

  static UnionOpaquePtr make(OpaquePtr<T> P) {
    UnionOpaquePtr OP = {P.getAsOpaquePtr()};
    return OP;
  }

  OpaquePtr<T> get() const { return OpaquePtr<T>::getFromOpaquePtr(Ptr); }
  operator OpaquePtr<T>() const { return get(); }

  UnionOpaquePtr &operator=(OpaquePtr<T> P) {
    Ptr = P.getAsOpaquePtr();
    return *this;
  }
};

} // namespace stone

namespace llvm {

template <class T> struct PointerLikeTypeTraits<stone::OpaquePtr<T>> {
  static constexpr int NumLowBitsAvailable = 0;

  static inline void *getAsVoidPointer(stone::OpaquePtr<T> P) {
    // FIXME: Doesn't work? return P.getAs< void >();
    return P.getAsOpaquePtr();
  }

  static inline stone::OpaquePtr<T> getFromVoidPointer(void *P) {
    return stone::OpaquePtr<T>::getFromOpaquePtr(P);
  }
};

} // namespace llvm
#endif
