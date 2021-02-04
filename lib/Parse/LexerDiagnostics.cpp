#include "stone/Parse/LexerDiagnostics.h"

enum LexerDiagID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Parse/LexerDiagnostics.def"
  MaxID
};
