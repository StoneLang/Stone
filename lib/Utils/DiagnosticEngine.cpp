#include "stone/Utils/DiagnosticEngine.h"
#include "stone/Utils/CoreDiagnostic.h"

using namespace stone;

namespace {
enum class LocalOptions {
  /// No options.
  none,

  /// The location of this diagnostic points to the beginning of the first
  /// token that the parser considers invalid.  If this token is located at the
  /// beginning of the line, then the location is adjusted to point to the end
  /// of the previous token.
  ///
  /// This behavior improves experience for "expected token X" diagnostics.
  PointsToFirstBadToken,

  /// After a fatal error subsequent diagnostics are suppressed.
  Fatal,
};

struct LocalDiagnostic {

  diag::Severity severity : 2;
  bool pointsToFirstBadToken : 1;
  bool isFatal : 1;

  constexpr LocalDiagnostic(diag::Severity severity, bool firstBadToken,
                            bool fatal)

      : severity(severity), pointsToFirstBadToken(firstBadToken),
        isFatal(fatal) {}
  constexpr LocalDiagnostic(diag::Severity severity, LocalOptions opts)
      : LocalDiagnostic(severity, opts == LocalOptions::PointsToFirstBadToken,
                        opts == LocalOptions::Fatal) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Utils/DiagnosticEngine.def"
  Max
};
} // end anonymous namespace

// TODO: categorization
static const constexpr LocalDiagnostic LocalDiagnostics[] = {
#define ERROR(ID, Options, Text, Signature)                                    \
  LocalDiagnostic(diag::Severity::Error, LocalOptions::Options),
#define WARNING(ID, Options, Text, Signature)                                  \
  LocalDiagnostic(diag::Severity::Warn, LocalOptions::Options),
#define NOTE(ID, Options, Text, Signature)                                     \
  LocalDiagnostic(diag::Severity::Note, LocalOptions::Options),
#define REMARK(ID, Options, Text, Signature)                                   \
  LocalDiagnostic(dia::Severity::Remark, LocalOptions::Options),
#include "stone/Utils/DiagnosticEngine.def"
};

static_assert((sizeof(LocalDiagnostics) / sizeof(LocalDiagnostic)) ==
                  LocalDiagID::Max,
              "array size mismatch");

DiagnosticEngine::DiagnosticEngine(const DiagnosticOptions &diagOpts)
    : diagOpts(diagOpts) {}

DiagnosticEngine::~DiagnosticEngine() {}

// TODO
bool DiagnosticEngine::HasError() { return false; }

void DiagnosticEngine::AddDiagnostic(Diagnostics *diagnostic) {

  assert(diagnostic && "Null diagnostic.");
  assert(diagnostic->GetMaxID() > 0 && "MaxID must be greater than 0.");

  /*
    if (entries.size() == 0) {
      diagnostics->firstID = 0;
      diagnostics->lastID = diagnostics->maxID;
    } else {
      // Get the last one from the entries
      // diagnostics->firstID = last->lastID + 1;
      diagnostics->lastID = diagnostics->firstID + diagnostics->lastID;
    }

  */
  // diagnostics->diagnosticID  = de.size() + 1;
  // entries.insert(diagnostics->diagnosticID, std::move(diagnostics));
}
void DiagnosticEngine::AddListener(DiagnosticListener *listerner) {}

void DiagnosticEngine::Print() {}
