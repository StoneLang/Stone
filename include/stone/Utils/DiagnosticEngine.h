#ifndef STONE_UTILS_DIAGNOSTICENGINE_H
#define STONE_UTILS_DIAGNOSTICENGINE_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"

#include "stone/Utils/DiagnosticListener.h"
#include "stone/Utils/DiagnosticPrinter.h"
#include "stone/Utils/LangOptions.h"
#include "stone/Utils/List.h"
#include "stone/Utils/SrcLoc.h"

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

namespace stone {

class DiagnosticEngine;
class InflightDiagnostic;
class DiagnosticListener;
class LangOptions;
class SrcMgr;
class StoredDiagnostic;

/// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in Diagnostics.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... argTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID ID;
};

namespace detail {
/// Describes how to pass a diagnostic argument of the given type.
///
/// By default, diagnostic arguments are passed by value, because they
/// tend to be small. Larger diagnostic arguments
/// need to specialize this class template to pass by reference.
template <typename T> struct PassArgument { typedef T type; };
} // namespace detail

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

/*
template <typename T> class CustomDianosticValue {
  T v;
public:
  CustomDiagnosticValue(T v) : v(v) {}
public:
  T GetValue() { return v; }
};
*/

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

class FixHint final {
public:
  /// Code that should be replaced to correct the error. Empty for an
  /// insertion hint.
  CharSrcRange removeRange;

  /// Code in the specific range that should be inserted in the insertion
  /// location.
  CharSrcRange insertFromRange;

  /// The actual code to insert at the insertion location, as a
  /// string.
  std::string codeToInsert;

  bool beforePreviousInsertions = false;

  /// Empty code modification hint, indicating that no code
  /// modification is known.
  FixHint() = default;

  bool IsNull() const { return !removeRange.isValid(); }

  /// Create a code modification hint that inserts the given
  /// code string at a specific location.
  static FixHint CreateInsertion(SrcLoc insertionLoc, StringRef code,
                                 bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.codeToInsert = std::string(code);
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that inserts the given
  /// code from \p FromRange at a specific location.
  static FixHint
  CreateInsertionFromRange(SrcLoc insertionLoc, CharSrcRange fromRange,
                           bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.insertFromRange = fromRange;
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that removes the given
  /// source range.
  static FixHint CreateRemoval(CharSrcRange removeRange) {
    FixHint fix;
    fix.removeRange = removeRange;
    return fix;
  }
  static FixHint CreateRemoval(SrcRange removeRange) {
    return CreateRemoval(CharSrcRange::getTokenRange(removeRange));
  }

  /// Create a code modification hint that replaces the given
  /// source range with the given code string.
  static FixHint CreateReplacement(CharSrcRange removeRange,
                                   llvm::StringRef code) {
    FixHint fix;
    fix.removeRange = removeRange;
    fix.codeToInsert = std::string(code);
    return fix;
  }

  static FixHint CreateReplacement(SrcRange removeRange, llvm::StringRef code) {
    return CreateReplacement(CharSrcRange::getTokenRange(removeRange), code);
  }
};
class DiagnosticMapping {
public:
};

class DiagnosticState {
  llvm::DenseMap<unsigned, DiagnosticMapping> diagMapping;

public:
  // "Global" configuration state that can actually vary between modules.

  // Ignore all warnings: -w
  unsigned ignoreAllWarnings : 1;

  // Enable all warnings.
  unsigned enableAllWarnings : 1;

  // Treat warnings like errors.
  unsigned warningsAsErrors : 1;

  // Treat errors like fatal errors.
  unsigned errorsAsFatal : 1;

  // Suppress warnings in system headers.
  unsigned suppressSystemWarnings : 1;

  // Map extensions to warnings or errors?
  diag::Severity extBehavior = diag::Severity::Ignore;

  DiagnosticState()
      : ignoreAllWarnings(false), enableAllWarnings(false),
        warningsAsErrors(false), errorsAsFatal(false),
        suppressSystemWarnings(false) {}

  using iterator = llvm::DenseMap<unsigned, DiagnosticMapping>::iterator;
  using const_iterator =
      llvm::DenseMap<unsigned, DiagnosticMapping>::const_iterator;

  /*
      void SetMapping(diag::kind Diag, DiagnosticMapping Info) {
        DiagMap[Diag] = Info;
      }

      DiagnosticMapping PickMapping(diag::kind Diag) const {
        return DiagMap.lookup(Diag);
      }

      DiagnosticMapping &GetOrAddMapping(diag::kind Diag);

      const_iterator begin() const { return DiagMap.begin(); }
      const_iterator end() const { return DiagMap.end(); }

  */
};

class DiagnosticStateMap {
public:
private:
  /// Represents a point in source where the diagnostic state was
  /// modified because of a pragma.
  ///
  /// 'Loc' can be null if the point represents the diagnostic state
  /// modifications done through the command-line.
  struct DiagnosticStatePoint {
    DiagnosticState *state;
    unsigned offset;
    DiagnosticStatePoint(DiagnosticState *state, unsigned offset)
        : state(state), offset(offset) {}
  };
};

/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final {

  friend class InFlightDiagnostic;
  friend class DiagnosticErrorTrap;
  friend class PartialDiagnostic;

  /// The
  unsigned int diagnosticSeen = 0;

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

  SrcMgr *sm = nullptr;

private:
  /// The location of the current diagnostic that is in flight.
  SrcLoc curDiagLoc;
  /// The ID of the current diagnostic that is in flight.
  ///
  /// This is set to std::numeric_limits<unsigned>::max() when there is no
  /// diagnostic in flight.
  unsigned curDiagID;

private:
  // Treat fatal errors like errors.
  bool fatalsAsError = false;

  // Suppress all diagnostics.
  bool suppressAllDiagnostics = false;

  // Elide common types of templates.
  bool elideType = true;

  // Print a tree when comparing templates.
  bool printTemplateTree = false;

  // Color printing is enabled.
  bool showColors = false;

  // Which overload candidates to show.
  // OverloadsShown ShowOverloads = Ovl_All;

  // Cap of # errors emitted, 0 -> no limit.
  unsigned errorLimit = 0;

  // Cap on depth of template backtrace stack, 0 -> no limit.
  // unsigned TemplateBacktraceLimit = 0;

  // Cap on depth of constexpr evaluation backtrace stack, 0 -> no limit.
  // unsigned ConstexprBacktraceLimit = 0;
  //
  //
  //
  /// The initial diagnostic state.
  DiagnosticState *firstDiagnosticState;

  /// The current diagnostic state.
  DiagnosticState *curDiagnosticState;

  /// The location at which the current diagnostic state was established.
  SrcLoc curDiagnosticStateLoc;

public:
  explicit DiagnosticEngine(const DiagnosticOptions &diagOpts,
                            SrcMgr *sm = nullptr);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;
  ~DiagnosticEngine();

public:
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

private:
  /// Sticky flag set to \c true when an error is emitted.
  bool errorOccurred;

  /// Sticky flag set to \c true when an "uncompilable error" occurs.
  /// I.e. an error that was not upgraded from a warning by -Werror.
  bool uncompilableErrorOccurred;

  /// Sticky flag set to \c true when a fatal error is emitted.
  bool fatalErrorOccurred;

  /// Indicates that an unrecoverable error has occurred.
  bool unrecoverableErrorOccurred;

  /// Counts for DiagnosticErrorTrap to check whether an error occurred
  /// during a parsing section, e.g. during parsing a function.
  unsigned trapNumErrorsOccurred;
  unsigned trapNumUnrecoverableErrorsOccurred;

  /// The level of the last diagnostic emitted.
  ///
  /// This is used to emit continuation diagnostics with the same level as the
  /// diagnostic that they follow.
  diag::Severity lastSeverity;

  /// Number of warnings reported
  unsigned numWarnings;

  /// Number of errors reported
  unsigned numErrors;

private:
  /// Grab the most-recently-added state point.
  DiagnosticState *GetCurDiagnosticState() const { return curDiagnosticState; }

public:
  bool HasSrcMgr() const { return sm != nullptr; }

  void SetSrgMgr(SrcMgr *sm) { this->sm = sm; }
  SrcMgr &GetSrcMgr() const {
    assert(sm && "SourceManager not set!");
    return *sm;
  }

  /// Copies the current DiagMappings and pushes the new copy
  /// onto the top of the stack.
  void PushMappings(SrcLoc loc);

  /// Pops the current DiagMappings off the top of the stack,
  /// causing the new top of the stack to be the active mappings.
  ///
  /// \returns \c true if the pop happens, \c false if there is only one
  /// DiagMapping on the stack.
  bool PopMappings(SrcLoc loc);

  void SetListener(DiagnosticListener *listener);

  // void AddArgument(DiagnosticArgument *argument);

  /// Specify a limit for the number of errors we should
  /// emit before giving up.
  ///
  /// Zero disables the limit.
  void SetErrorLimit(unsigned limit) { errorLimit = limit; }

  /// When set to true, any unmapped warnings are ignored.
  ///
  /// If this and WarningsAsErrors are both set, then this one wins.
  void SetIgnoreAllWarnings(bool status) {
    GetCurDiagnosticState()->ignoreAllWarnings = status;
  }
  // bool GetIgnoreAllWarnings() const {
  //  return GetCurDiagState()->IgnoreAllWarnings;
  // }
  /// Emit the current diagnostic and clear the diagnostic state.
  ///
  /// \param Force Emit the diagnostic regardless of suppression settings.
  bool EmitCurrentDiagnostic(bool force = false);

  unsigned GetCurrentDiagID() const { return curDiagID; }

  SrcLoc GetCurrentDiagLoc() const { return curDiagLoc; }
};

/// RAII class that determines when any errors have occurred
/// between the time the instance was created and the time it was
/// queried.
///
/// Note that you almost certainly do not want to use this. It's usually
/// meaningless to ask whether a particular scope triggered an error message,
/// because error messages outside that scope can mark things invalid (or cause
/// us to reach an error limit), which can suppress errors within that scope.
class DiagnosticErrorTrap {
  DiagnosticEngine &de;
  unsigned numErrors;
  unsigned numUnrecoverableErrors;

public:
  explicit DiagnosticErrorTrap(DiagnosticEngine &de) : de(de) { reset(); }

  /// Determine whether any errors have occurred since this
  /// object instance was created.
  bool hasErrorOccurred() const { return de.trapNumErrorsOccurred > numErrors; }

  /// Determine whether any unrecoverable errors have occurred since this
  /// object instance was created.
  bool hasUnrecoverableErrorOccurred() const {
    return de.trapNumUnrecoverableErrorsOccurred > numUnrecoverableErrors;
  }

  /// Set to initial state of "no errors occurred".
  void reset() {
    numErrors = de.trapNumErrorsOccurred;
    numUnrecoverableErrors = de.trapNumUnrecoverableErrorsOccurred;
  }
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
    assert(de && "InflightDiagnostic requires a valid DiagnosticEngine!");

    // de->diagnosticRanges.clear();
    // de->diagnosticFixHints.clear();
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
    // the DiagnosticEngine object.
    FlushCounts();
    // Process the diagnostic.
    // bool result = de->EmitCurrentDiagnostic(IsForceEmit);

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
