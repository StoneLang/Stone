#ifndef STONE_DRIVER_DRIVERSTATS_H
#define STONE_DRIVER_DRIVERSTATS_H

#include "stone/Core/Stats.h"

namespace stone {
namespace driver {
class Driver;
class DriverStats final : public Stats {
  const Driver& driver;

 public:
  DriverStats(const Driver& driver);
  void Print() const override;
};
}  // namespace driver
}  // namespace stone
#endif
