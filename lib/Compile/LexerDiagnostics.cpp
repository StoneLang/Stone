#include "stone/Compile/LexerDiagnostics.h"

enum LexerDiagID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Compile/LexerDiagnostics.def"
  MaxID
};
