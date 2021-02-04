#include "stone/AST/ASTDiagnostics.h"

using namespace stone;
using namespace stone::syn;

enum ASTDiagID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/AST/ASTDiagnostics.def"
  MaxID
};
