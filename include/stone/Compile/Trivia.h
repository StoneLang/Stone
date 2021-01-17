#ifndef STONE_COMPILE_TRIVIA_H
#define STONE_COMPILE_TRIVIA_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "stone/Core/LLVM.h"

namespace stone {
namespace analysis {
enum class TriviaKind { GarbageText };

class TriviaPiece final {
  TriviaKind kind;
  unsigned length;

 public:
  TriviaPiece(TriviaKind kind, unsigned length) : kind(kind), length(length) {}

  TriviaKind GetKind() const { return kind; }

  /// Return the text of the trivia.
  unsigned GetLength() const { return length; }

  void ExtendLength(unsigned len) { length += len; }

  static size_t GetTotalLength(llvm::ArrayRef<TriviaPiece> pieces) {
    size_t Len = 0;
    for (auto &p : pieces) Len += p.GetLength();
    return Len;
  }

  bool operator==(const TriviaPiece &other) const {
    return kind == other.kind && length == other.length;
  }
  bool operator!=(const TriviaPiece &other) const { return !(*this == other); }
};

using TriviaPieces = llvm::SmallVector<TriviaPiece, 3>;

struct Trivia {
  TriviaPieces pieces;

  /// Get the begin iterator of the pieces.
  TriviaPieces::const_iterator begin() const { return pieces.begin(); }

  /// Get the end iterator of the pieces.
  TriviaPieces::const_iterator end() const { return pieces.end(); }

  /// Clear pieces.
  void clear() { pieces.clear(); }

  /// Returns true if there are no pieces in this Trivia collection.
  bool empty() const { return pieces.empty(); }

  /// Return the number of pieces in this Trivia collection.
  size_t size() const { return pieces.size(); }

  size_t getLength() const { return TriviaPiece::GetTotalLength(pieces); }

  void push_back(TriviaKind kind, unsigned length) {
    pieces.emplace_back(kind, length);
  }

  void AppendOrSquash(TriviaKind kind, unsigned length) {
    if (empty() || pieces.back().GetKind() != kind) {
      push_back(kind, length);
    } else {
      pieces.back().ExtendLength(length);
    }
  }

  bool operator==(const Trivia &other) const {
    if (pieces.size() != other.size()) {
      return false;
    }

    for (size_t i = 0; i < pieces.size(); ++i) {
      if (pieces[i] != other.pieces[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(const Trivia &other) const { return !(*this == other); }
};
}  // namespace analysis
}  // namespace stone

#endif
