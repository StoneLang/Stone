#ifndef STONE_DRIVER_DRIVEROPTIONS_H
#define STONE_DRIVER_DRIVEROPTIONS_H

#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

class InputFile final {};

class OutputFile final {};

class DriverOptions final : public SessionOptions {
 public:
  DriverOptions() {}

 public:
};
}  // namespace driver
}  // namespace stone

#endif
