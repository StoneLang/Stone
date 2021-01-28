#include "stone/Driver/DriverStats.h"

#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::driver;

DriverStats::DriverStats(const char* name, const Driver& driver)
    : Stats(name), driver(driver) {}

void DriverStats::Print() const {
  if (driver.driverOpts.printStats) {
    // cos << GetName() << '\n';
    return;
  }

  // Print out information about the SafeList<Job>
}
