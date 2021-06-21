#include "stone/Driver/Compilation.h"
#include "stone/Basic/Ret.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptSpecifier.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::driver;

using namespace llvm::opt;

Compilation::Compilation(Driver &driver) : driver(driver) {
  stats.reset(new CompilationStats(*this, driver));
  driver.GetStatEngine().Register(stats.get());
  if (driver.driverOpts.printStats) {
  }
}

Compilation::~Compilation() {}

void Compilation::AddJob(const Job *job) { jobs.Add(job); }

bool Compilation::PurgeFile(const char *name, bool issueErrors) const {
  return true;
}

bool Compilation::PurgeFiles(const llvm::opt::ArgStringList &files,
                             bool issueErrors) const {
  return true;
}

int Compilation::ExecuteJob(const Job &job, const Job *&fallBackJob) const {
  return 0;
}

void Compilation::ExecuteJobs(
    llvm::SmallVectorImpl<std::pair<int, const Job *>> &fallBackJob) const {}

/// Asks the Compilation to perform the Jobs which it knows about.
///
/// \param TQ The TaskQueue used to schedule jobs for execution.
///
/// \returns result code for the Compilation's Jobs; 0 indicates success and
/// -2 indicates that one of the Compilation's Jobs crashed during execution
// CompilationResult Run(std::unique_ptr<driver::TaskQueue> &&queue) {
//	return;
//}

void CompilationStats::Print() {
  if (compilation.GetDriver().GetDriverOptions().printStats) {

    GetBasic().Out() << GetName() << '\n';
    return;
  }
}
