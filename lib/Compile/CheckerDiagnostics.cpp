#include "stone/Compile/CheckerDiagnostics.h"

enum CheckerDiagID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Compile/CheckerDiagnostics.def"
  MaxID
};
