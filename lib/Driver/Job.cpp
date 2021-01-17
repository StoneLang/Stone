#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

Job::~Job() {}

void Job::Print(llvm::raw_ostream &os, const char *terminator, bool quote,
                CrashCondition *crash) const {}

int Job::AsyncExecute(llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects,
                      std::string *errMsg, bool *failed) const {
  return 0;
}

int Job::SyncExecute(llvm::ArrayRef<llvm::Optional<llvm::StringRef>> redirects,
                     std::string *errMsg, bool *failed) const {
  return 0;
}
