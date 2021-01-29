#ifndef STONE_DRIVER_DRIVEROPTIONS_H
#define STONE_DRIVER_DRIVEROPTIONS_H

#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

// TODO: a lot of what is in DriverOutputProfile can go here
class DriverOptions final : public SessionOptions {
 public:
  bool printJobs = false;
  bool showJobLifecycle = false;

 public:
  DriverOptions() {}
};
}  // namespace driver
}  // namespace stone

#endif
