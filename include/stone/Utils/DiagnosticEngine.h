#ifndef STONE_UTILS_DIAGNOSTICENGINE_H
#define STONE_UTILS_DIAGNOSTICENGINE_H

#include <cassert>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"

#include "stone/Utils/DiagnosticFixHint.h"
#include "stone/Utils/DiagnosticListener.h"
#include "stone/Utils/DiagnosticPrinter.h"
#include "stone/Utils/LangOptions.h"
#include "stone/Utils/List.h"
#include "stone/Utils/SrcLoc.h"

namespace stone {

class Diagnostics;
class DiagnosticEngine;
class InflightDiagnostic;

enum DiagID : uint32_t;

enum FixHintID : uint32_t;

class Diagnostics {
  friend DiagnosticEngine;
  // bool isActive;
protected:
  unsigned int uniqueID = 0;
  unsigned maxID = 0;
  unsigned firstID = 0;
  unsigned lastID = 0;

  // llvm::DenseMap<unsigned, Diagnostic> entries;
  // List<Diagnostic> entries;

public:
  unsigned GetUniqueID() const { return uniqueID; }

protected:
  // Only for Diagnostics
  void Init();
  unsigned GetMaxID() const { return maxID; }
  unsigned GetFirstID() const { return firstID; }
  unsigned GetLastID() const { return lastID; }

  // const DiagnosticLine DiagnosticLines[100];
public:
};

enum class DiagnosticArgumentKind {
  /// std::string
  STDStr,

  /// const char *
  CStr,

  /// llvm::StringRef
  LLVMStr,

  /// int
  SInt,

  /// unsigned
  UInt,

  /// custom argument
  Custom,
};

class DiagnosticArgument {
  DiagnosticArgumentKind kind;

public:
  DiagnosticArgument(DiagnosticArgumentKind kind) : kind(kind) {}

public:
  DiagnosticArgumentKind GetKind() { return kind; }
};
class CustomDiagnosticArgument : public DiagnosticArgument {
public:
  CustomDiagnosticArgument()
      : DiagnosticArgument(DiagnosticArgumentKind::Custom) {}
};
/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final {

  friend class InFlightDiagnostic;
  /// The
  unsigned int diagnosticSeen = 0;

  /// The maximum diagnostic messages per diagnostic
  // unsigned int maxDiagnosticMessages = 1000;
  llvm::DenseMap<unsigned int, Diagnostics *> diagnostics;

  // TODO: This may have to be on the dagnostics
  /// If valid, provides a hint with some code to insert, remove,
  /// or modify at a particular position.
  llvm::SmallVector<FixHint, 8> fixHints;

  /// The diagnostic listeners(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticListener *, 2> listeners;

  /// The diagnostic listeners(s) that will be responsible for actually
  /// emitting diagnostics.
  // llvm::SmallVector<CustomDiagnosticArgument *, 2> customArguments;

  const DiagnosticOptions &diagOpts;

public:
  explicit DiagnosticEngine(const DiagnosticOptions &diagOpts);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;
  ~DiagnosticEngine();

public:
  /// Owns the Diagnostics
  // NOTE: when you add, check for existing, calculate id, start, and end and
  // then load message; diagnostic.messageID = diagnostics.size() +1;
  ////TODO: remove this note: (d1Start = 1, d1End = d1Start + max)
  // (d2Start = d1End + 1  , d2End = d1End + max)
  // update: use maxMessages from Diagnostic to calculate startMsgID, and
  // endMsgID
  void AddDiagnostic(Diagnostics *diagnostic);

  /// Remove a specific DiagnosticConsumer.
  // void RemoveDiagnostic(Diagnostics *diagnostic) {
  //    diagnostics.erase(
  //        std::remove(diagnostics.begin(), Consumers.end(), diagnostic));
  //}

  void AddListener(DiagnosticListener *listener);
  void AddArgument(DiagnosticArgument *argument);

  bool HasError();
  void Print();

  /// Issue the message to the client.
  ///
  /// This actually returns an instance of InflightDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  inline InflightDiagnostic Issue(SrcLoc loc, unsigned diagID);
  inline InflightDiagnostic Issue(unsigned diagID);
};

class InflightDiagnostic final {
  friend class DiagnosticEngine;
  // friend class PartialDiagnostic;

  mutable DiagnosticEngine *de = nullptr;
  mutable unsigned numArgs = 0;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool isActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool isForceEmit = false;

  InflightDiagnostic() = default;

  explicit InflightDiagnostic(DiagnosticEngine *de) : de(de), isActive(true) {
    assert(de && "InflightDiagnostic requires a valid DiagnosticsEngine!");

    // diagnostics->diagnosticRanges.clear();
    // diagnostics->diagnosticFixHints.clear();
  }

public:
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of InflightDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  inline InflightDiagnostic
  Diagnose(const SrcLoc loc, const unsigned diagnosticID, const unsigned msgID);

  inline InflightDiagnostic Diagnose(const unsigned diagnosticID,
                                     const unsigned msgID);

protected:
  void FlushCounts() {}

  /// Clear out the current diagnostic.
  void Clear() const {}

  /// Determine whether this diagnostic is still active.
  bool IsActive() const { return isActive; }

  bool Emit() {
    // If this diagnostic is inactive, then its soul was stolen by the copy ctor
    // (or by a subclass, as in SemaInflightDiagnostic).
    if (!IsActive())
      return false;

    // When emitting diagnostics, we set the final argument count into
    // the DiagnosticsEngine object.
    FlushCounts();
    // Process the diagnostic.
    // bool result = diagObj->EmitCurrentDiagnostic(IsForceEmit);

    // This diagnostic is dead.
    Clear();
    // return Result;
    return false;
  }

public:
  /// Copy constructor.  When copied, this "takes" the diagnostic info from the
  /// input and neuters it.
  InflightDiagnostic(const InflightDiagnostic &inflight) {}
  InflightDiagnostic &operator=(const InflightDiagnostic &) = delete;

  /// Emits the diagnostic.
  ~InflightDiagnostic() { Emit(); }
};

/*
/// Register a value for the flag in the current diagnostic. This
/// value will be shown as the suffix "=value" after the flag name. It is
/// useful in cases where the diagnostic flag accepts values (e.g.,
/// -Rpass or -Wframe-larger-than).
inline const InflightDiagnostic &operator<<(const InflightDiagnostic &inflight,
                                           const AddFlagValue V) {
  inflight.AddFlagValue(V.Val);
  return inflight;
}
*/

inline const InflightDiagnostic &operator<<(const InflightDiagnostic &inflight,
                                            llvm::StringRef data) {
  // inflight.AddString(data);
  return inflight;
}

/*
inline const InflightDiagnostic &operator<<(const InflightDiagnostic &inflight,
                                           const char *Str) {
  inflight.AddTaggedVal(reinterpret_cast<intptr_t>(Str),
                  DiagnosticArgumentKind::CStr);
  return inflight;
}

inline const InflightDiagnostic &operator<<(const InflightDiagnostic &inflight,
int data) { inflight.AddTaggedVal(data, DiagnosticArgumentKind::SInt); return
inflight;
}
*/

} // namespace stone
#endif
