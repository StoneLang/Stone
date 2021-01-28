#include "stone/Driver/DriverStats.h"

#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::driver;

DriverStats::DriverStats(const Driver& driver)
    : Stats("driver statistics:"), driver(driver) {}

void DriverStats::Print() {
  if (driver.driverOpts.printStats) {
    cos.UseGreen();
    cos << GetName() << '\n';
    return;
  }
}
