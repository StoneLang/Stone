#include "stone/Core/Ret.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::file;
using namespace stone::driver;

namespace stone {
namespace driver {
struct JobBuilder final {
  /// Build jobs for compiling
  static int BuildJobsForCompile(Driver& driver);
  static Job* BuildJobForCompile(Driver& driver, const InputFile& input);

  /// Build jobs for linking
  static int BuildJobForLinking(Driver& driver);
  static Job* BuildJobForLinkingImpl(Driver& driver);
  static Job* BuildJobForStaticLinking(Driver& driver);
  static Job* BuildJobForDynamicLinking(Driver& driver);

  /// Build a jobs for compiling, and linking.
  static int BuildJobsForExecutable(Driver& driver);
};
}  // namespace driver
}  // namespace stone

int JobBuilder::BuildJobsForCompile(Driver& driver) {
  assert(driver.GetMode().IsCompileOnly() &&
         "Can only be called directly for compiling only.");

  for (const auto& input : driver.GetDriverOptions().inputs) {
    auto job = JobBuilder::BuildJobForCompile(driver, input);
    driver.AddJobForCompilation(job);
  }
  return ret::ok;
}

Job* JobBuilder::BuildJobForCompile(Driver& driver, const InputFile& input) {
  assert(driver.GetMode().CanCompile() &&
         "The 'mode-type' does not support compiling.");

  assert(input.first == FileType::Stone && "Incorrect file for compiling.");
  auto tool = driver.GetToolChain().PickTool(JobType::Compile);
  assert(tool && "Could not find a tool for CompileJob.");
  // return tool->CreateJob(driver.GetCompilation(), std::move(cmdOutput),
  //                               driver.GetOutputProfile());
  return nullptr;
}

int JobBuilder::BuildJobForLinking(Driver& driver) {
  assert(driver.GetMode().IsLinkOnly() &&
         "Can only be called directly for linking only");

  auto job = JobBuilder::BuildJobForLinkingImpl(driver);
  for (const auto& input : driver.GetDriverOptions().inputs) {
    assert(input.first == FileType::Object && "Incorrect file for linking.");
    job->AddInput(input);
  }
  driver.AddJobForCompilation(job);
  return ret::ok;
}

Job* JobBuilder::BuildJobForLinkingImpl(Driver& driver) {
  assert(driver.GetMode().CanLink() &&
         "The 'mode-type' does not support linking.");

  Job* result = nullptr;
  switch (driver.GetOutputProfile().linkType) {
    case LinkType::StaticLibrary:
      result = JobBuilder::BuildJobForStaticLinking(driver);
      break;
    case LinkType::DynamicLibrary:
      result = JobBuilder::BuildJobForDynamicLinking(driver);
      break;
    default:
      break;
  }
  return result;
}

Job* JobBuilder::BuildJobForStaticLinking(Driver& driver) {
  auto tool = driver.GetToolChain().PickTool(JobType::StaticLink);
  assert(tool && "Could not find a tool for static linking.");

  // result = driver.GetCompilation().CreateJob<StaticLinkJob>(
  //    driver.GetCompilation(), driver.GetOutputProfile().RequiresLTO(),
  //    driver.GetOutputProfile().linkType);
  //
  return nullptr;
}
Job* JobBuilder::BuildJobForDynamicLinking(Driver& driver) {
  auto tool = driver.GetToolChain().PickTool(JobType::DynamicLink);
  assert(tool && "Could not find a tool for dynamic linking.");
  // result = driver.GetCompilation().CreateJob<DynamicLinkJob>(
  //    driver.GetCompilation(), driver.GetOutputProfile().RequiresLTO(),
  //    driver.GetOutputProfile().linkType);
  // TODO: get the tool from the ToolChain and pass to CreateJob?
  //
  return nullptr;
}

int JobBuilder::BuildJobsForExecutable(Driver& driver) {
  auto link = JobBuilder::BuildJobForLinkingImpl(driver);
  for (const auto& input : driver.GetDriverOptions().inputs) {
    auto compile = JobBuilder::BuildJobForCompile(driver, input);
    link->AddDep(compile);
  }
  driver.AddJobForCompilation(link);
  return ret::ok;
}

int Driver::BuildJobs() {
  llvm::PrettyStackTraceString CrashInfo("Building compilation jobs.");

  if (GetDriverOptions().inputs.empty()) {
    Out() << "D(SrcLoc(), msg::error_no_input_files)" << '\n';
    return ret::err;
  }
  switch (GetMode().GetKind()) {
    case ModeKind::Check:
    case ModeKind::EmitLibrary:
    case ModeKind::EmitObject:
    case ModeKind::EmitAssembly:
    case ModeKind::EmitModuleOnly:
    case ModeKind::EmitIR:
    case ModeKind::EmitBC:
    case ModeKind::Parse:
      return JobBuilder::BuildJobsForCompile(*this);
      break;
    case ModeKind::EmitExecutable:
      return JobBuilder::BuildJobsForExecutable(*this);
      break;
    case ModeKind::Link:
      return JobBuilder::BuildJobForLinking(*this);
    default:
      return ret::err;
      break;
  }
}
