#include "stone/Driver/DriverDiagnostics.h"

using namespace stone;
using namespace stone::driver;

/*
enum DriverDiagnostics::DiagID : uint32_t {
  firstID,
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Driver/DriverDiagnostics.def"
  lastID
};
*/
DriverDiagnostics::DriverDiagnostics() { maxID = 200; }
