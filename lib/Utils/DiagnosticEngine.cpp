#include "stone/Utils/DiagnosticEngine.h"
#include "stone/Utils/CoreDiagnostic.h"

using namespace stone;

enum class stone::DiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Utils/DiagnosticEngine.def"
};
static_assert(static_cast<uint32_t>(stone::DiagID::invalid_diagnostic) == 0,
              "0 is not the invalid diagnostic ID");

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace stone {
namespace diag {
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  detail::DiagWithArguments<void Signature>::type ID = {DiagID::ID};
#include "stone/Utils/DiagnosticEngine.def"
} // end namespace diag
} // end namespace stone

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
