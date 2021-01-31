#include "stone/Driver/JobBuilder.h"

#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::file;
using namespace stone::driver;

bool JobBuilder::BuildJobs(Driver& driver) {
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

bool JobBuilder::BuildJobsForCompile(Driver& driver) {
  assert(driver.GetMode().IsCompileOnly() &&
         "Can only be called directly for compiling only.");

  for (const auto& input : driver.GetDriverOptions().inputs) {
    auto job = JobBuilder::BuildJobForCompile(driver, input);
    driver.AddJobForCompilation(job);
  }
  return true;
}

Job* JobBuilder::BuildJobForCompile(Driver& driver, const InputFile& input) {
  return nullptr;
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
