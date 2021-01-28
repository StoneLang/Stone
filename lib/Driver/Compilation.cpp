#include "stone/Driver/Compilation.h"

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
#include "stone/Core/Ret.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;
using namespace stone::driver;

using namespace llvm::opt;

Compilation::Compilation(Driver &driver) : driver(driver) {
  stats.reset(new CompilationStats("Compilation", *this));
  driver.GetStatEngine().Register(stats.get());
}

Compilation::~Compilation() {}

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

int Compilation::Run() { return 0; }

void CompilationStats::Print() const {
  if (compilation.GetDriver().GetDriverOptions().printStats) {
    os << GetName() << '\n';
    return;
  }
}
