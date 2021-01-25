#ifndef STONE_DRIVER_DRIVEROPTIONS_H
#define STONE_DRIVER_DRIVEROPTIONS_H

#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

class DriverOptions final : public SessionOptions {
 public:
  bool showActivities = false;
  bool showJobs = false;

 public:
  DriverOptions() {}
};
}  // namespace driver
}  // namespace stone

#endif
