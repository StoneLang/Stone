#ifndef STONE_DRIVER_DRIVERDIAGNOSTICS_H
#define STONE_DRIVER_DRIVERDIAGNOSTICS_H

#include <memory>

#include "stone/Utils/Diagnostics.h"

namespace stone {
namespace driver {
class DriverDiagnostics final : public Diagnostics {
public:
  enum DiagID : uint32_t;

public:
  DriverDiagnostics();
};
} // namespace driver
} // namespace stone

#endif
