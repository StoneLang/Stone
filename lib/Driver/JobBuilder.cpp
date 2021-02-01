#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::file;
using namespace stone::driver;

namespace stone {
namespace driver {
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
bool JobBuilder::BuildJobsForCompile(Driver& driver) {
  assert(driver.GetMode().IsCompileOnly() &&
         "Can only be called directly for compiling only.");

  // Pick a tool
  for (const auto& input : driver.GetDriverOptions().inputs) {
    auto job = JobBuilder::BuildJobForCompile(driver, input);
    driver.AddJobForCompilation(job);
  }
  return true;
}

Job* JobBuilder::BuildJobForCompile(Driver& driver, const InputFile& input) {
  assert(input.first == FileType::Stone && "Incorrect file for compiling.");
}

bool JobBuilder::BuildJobForLinking(Driver& driver) {
  assert(driver.GetMode().IsLinkOnly() &&
         "Can only be called directly for linking only");

  auto job = JobBuilder::BuildJobForLinkingImpl(driver);
  for (const auto& input : driver.GetDriverOptions().inputs) {
    assert(input.first == FileType::Object && "Incorrect file for linking.");
    job->AddInput(input);
  }
  driver.AddJobForCompilation(job);
  return true;
}

Job* JobBuilder::BuildJobForLinkingImpl(Driver& driver) { return nullptr; }

bool JobBuilder::BuildJobsForExecutable(Driver& driver) {
  for (const auto& input : driver.GetDriverOptions().inputs) {
    auto job = JobBuilder::BuildJobForCompile(driver, input);
  }
}

bool Driver::BuildJobs(Driver& driver) {
  switch (driver.GetMode().GetKind()) {
    case ModeKind::Check:
    case ModeKind::EmitLibrary:
    case ModeKind::EmitObject:
    case ModeKind::EmitAssembly:
    case ModeKind::EmitModuleOnly:
    case ModeKind::EmitIR:
    case ModeKind::EmitBC:
    case ModeKind::Parse:
      return JobBuilder::BuildJobsForCompile(driver);
      break;
    case ModeKind::EmitExecutable:
      return JobBuilder::BuildJobsForExecutable(driver);
      break;
    case ModeKind::Link:
      return JobBuilder::BuildJobForLinking(driver);
    default:
      return false;
      break;
  }
}
