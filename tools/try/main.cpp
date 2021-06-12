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

#include "stone/Basic/List.h"
#include "stone/Basic/SrcLoc.h"

class DiagnosticEngine;

struct DiagnosticEntry {};

class Diagnostic {
  friend DiagnosticEngine;
  const char *name = nullptr;
  const char *path = nullptr;

protected:
  unsigned diagID = 0;
  unsigned firstID = 0;
  unsigned lastID = 0;
  unsigned maxID = 0;

  llvm::DenseMap<unsigned, DiagnosticEntry> entries;

public:
  Diagnostic(const char *name, const char *path) : name(name), path(path) {}

public:
  const char *GetName() { return name; }
  const char *GetPath() { return path; }

  unsigned GetFirstID() { return firstID; }
  unsigned GetMaxID() { return maxID; }
  unsigned GetLastID() { return lastID; }

public:
  virtual void Init() = 0;
};

class DriverDiagnostic : public Diagnostic {
public:
  DriverDiagnostic() : Diagnostic("DriverDiagnostic", "DriverDiagnostic.def") {
    maxID = 200;
  }
  void Init() override {}
};

class DiagnosticEngine final {

  /// The maximum diagnostic messages per diagnostic
  // unsigned int maxDiagnosticMessages = 1000;
  llvm::SmallVector<std::pair<unsigned, Diagnostic *>, 5> diags;

public:
  void AddDiagnostic(Diagnostic *diag) {
    assert(diag && "Null diagnostic.");
    assert(diag->GetMaxID() > 0 && "MaxID must be greater than 0.");

    if (diags.size() == 0) {
      diag->firstID = 0;
      diag->lastID = diag->maxID;
    } else {
      // Get the last one from the entries
      // diag->firstID = last->lastID + 1;
      diag->lastID = diag->firstID + diag->lastID;
    }
  }

  void Issue(unsigned diagid, unsigned msgid) {}
};

int main() {

  std::unique_ptr<DriverDiagnostic> driverDiagnostic(new DriverDiagnostic());

  DiagnosticEngine engine;
  engine.AddDiagnostic(driverDiagnostic.get());

  return 0;
}
