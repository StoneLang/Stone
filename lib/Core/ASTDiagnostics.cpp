#include "stone/Core/ASTDiagnostics.h"

using namespace stone;
using namespace stone::syn;

enum ASTDiagID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Core/ASTDiagnostics.def"
  MaxID
};
