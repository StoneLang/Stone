#ifndef STONE_CORE_DIAGNOSTICS_H
#define STONE_CORE_DIAGNOSTICS_H

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
#include "stone/Core/DiagnosticOptions.h"
#include "stone/Core/SrcLoc.h"

namespace stone {

class DiagnosticEngine;
class DiagnosticBuilder;

class FixIt final {};

class DiagnosticPrinter {};
class DiagnosticListener {};

class Diagnostics {
  friend DiagnosticEngine;
  // bool isActive;
 protected:
  unsigned int uniqueID = 0;
  unsigned int maxMessages = 0;
  unsigned int startID = 0;
  unsigned int endID = 0;

 public:
  unsigned int GetUniqueID() const { return uniqueID; }

 protected:
  // Only for Diagnostics
  void Init();
  unsigned int GetStartID() const { return startID; }
  unsigned int GetEndID() const { return endID; }

 public:
};

enum class DiagnosticLevel {
  None,
  Ignore,
  Note,
  Remark,
  Warning,
  Error,
  Fatal
};

enum class DiagnositicArgumentKind {
  /// std::string
  STDStr,

  /// const char *
  CStr,

  /// int
  SInt,

  /// unsigned
  UInt,

  /// custom argument
  Custom,
};

class CustomDiagnosticArgument {};
/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final {
  /// The
  unsigned int diagnosticSeen = 0;

  /// The maximum diagnostic messages per diagnostic
  // unsigned int maxDiagnosticMessages = 1000;
  llvm::DenseMap<unsigned int, std::unique_ptr<Diagnostics>> entries;

 public:
  explicit DiagnosticEngine(const DiagnosticOptions &diagOpts,
                            DiagnosticListener *listener = nullptr,
                            bool ownsListener = true);

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
  void AddDiagnostics(std::unique_ptr<Diagnostics> diagnostics);

  // void AddDiagnosticListener(std::unique_ptr<DiagnosticListener> diagnostic);
  //
  bool HasError();
};

class DiagnosticBuilder final {
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

  DiagnosticBuilder() = default;

  explicit DiagnosticBuilder(DiagnosticEngine *de) : de(de), isActive(true) {
    assert(de && "DiagnosticBuilder requires a valid DiagnosticsEngine!");

    // diagnostics->diagnosticRanges.clear();
    // diagnostics->diagnosticFixIts.clear();
  }

 public:
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of DiagnosticBuilder which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  inline DiagnosticBuilder Diagnose(const SrcLoc loc,
                                    const unsigned diagnosticID,
                                    const unsigned messageID);

  inline DiagnosticBuilder Diagnose(const unsigned diagnosticID,
                                    const unsigned messageID);
};

}  // namespace stone
#endif
