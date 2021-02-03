#ifndef STONE_UTILS_DIAGNOSTICS_H
#define STONE_UTILS_DIAGNOSTICS_H

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
#include "stone/Utils/DiagnosticOptions.h"
#include "stone/Utils/LangOptions.h"
#include "stone/Utils/List.h"
#include "stone/Utils/SrcLoc.h"

namespace stone {

class Diagnostics;
class DiagnosticEngine;
class DiagnosticBuilder;

enum class DiagnosticLevel {
  None,
  Ignore,
  Note,
  Remark,
  Warning,
  Error,
  Fatal
};

class FixIt final {
 public:
};

class DiagnosticPrinter {};

class DiagnosticListener {
 protected:
  unsigned numWarnings = 0;  ///< Number of warnings reported
  unsigned numErrors = 0;    ///< Number of errors reported

 public:
  DiagnosticListener() = default;
  virtual ~DiagnosticListener();

  unsigned GetNumErrors() const { return numErrors; }
  unsigned GetNumWarnings() const { return numWarnings; }

  /// Clear all warnings and errors.
  virtual void Clear() { numWarnings = numErrors = 0; }

  /// Callback to inform the diagnostic client that processing
  /// of a source file is beginning.
  ///
  /// Note that diagnostics may be emitted outside the processing of a source
  /// file, for example during the parsing of command line options. However,
  /// diagnostics with source range information are required to only be emitted
  /// in between BeginSourceFile() and EndSourceFile().
  ///
  /// \param LangOpts The language options for the source file being processed.
  /// \param PP The preprocessor object being used for the source; this is
  /// optional, e.g., it may not be present when processing AST source files.
  virtual void BeginSourceFile(const LangOptions &langOpts) {}

  /// Callback to inform the diagnostic client that processing
  /// of a source file has ended.
  ///
  /// The diagnostic client should assume that any objects made available via
  /// BeginSourceFile() are inaccessible.
  virtual void EndSourceFile() {}

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual void Finish() {}

  /// Indicates whether the diagnostics handled by this
  /// DiagnosticConsumer should be included in the number of diagnostics
  /// reported by DiagnosticsEngine.
  ///
  /// The default implementation returns true.
  virtual bool IncludeInDiagnosticCounts() const;

  /// Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  ///
  /// The default implementation just keeps track of the total number of
  /// warnings and errors.
  virtual void HandleDiagnostic(DiagnosticLevel level,
                                const Diagnostics &diagnostics);
};

class FakeDiagnosticListener final : public DiagnosticListener {
 public:
};
struct Diagnostic final {
 public:
  Diagnostic() {}
};

class Diagnostics {
  friend DiagnosticEngine;
  // bool isActive;
 protected:
  unsigned int uniqueID = 0;
  unsigned int maxMessages = 0;
  unsigned int startID = 0;
  unsigned int endID = 0;

  enum MsgID : uint32_t;

  // llvm::DenseMap<unsigned, Diagnostic> entries;

  // List<Diagnostic> entries;

 public:
  unsigned GetUniqueID() const { return uniqueID; }

 protected:
  // Only for Diagnostics
  void Init();
  unsigned int GetStartID() const { return startID; }
  unsigned int GetEndID() const { return endID; }

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
  void Register(std::unique_ptr<Diagnostics> diagnostics);

  // void AddDiagnosticListener(std::unique_ptr<DiagnosticListener> diagnostic);
  //
  bool HasError();

  void Print();

  void AddCustomArgument(const CustomDiagnosticArgument *argument);
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
