#include "stone/Driver/DriverDiagnostics.h"

using namespace stone;
using namespace stone::driver;

enum DriverMsgID : uint32_t {
#define DIAG(Kind, ID, Options, Text, Signature) ID,
#include "stone/Driver/DriverDiagnostics.def"
  MaxID
};
