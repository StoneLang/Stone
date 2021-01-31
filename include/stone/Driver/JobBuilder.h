#ifndef STONE_DRIVER_JOBBUILDER_H
#define STONE_DRIVER_JOBBUILDER_H

#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {
class Job;
class Driver;

struct JobBuilder final {
  /// Build jobs entry point
  static bool BuildJobs(Driver& driver);

  /// Build jobs for compiling
  static bool BuildJobsForCompile(Driver& driver);
  static Job* BuildJobForCompile(Driver& driver, const InputFile& input);

  /// Build jobs for linking
  static bool BuildJobForLinking(Driver& driver);
  static Job* BuildJobForLinkingImpl(Driver& driver);

  /// Build a jobs for compiling, and linking.
  static bool BuildJobsForExecutable(Driver& driver);
};
}  // namespace driver
}  // namespace stone
#endif
