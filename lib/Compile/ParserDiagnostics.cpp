#include "stone/Compile/ParserDiagnostics.h"

enum ParserDiagID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Compile/ParserDiagnostics.def"
  MaxID
};
