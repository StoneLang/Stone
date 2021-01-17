#include "stone/Driver/Run.h"

#include <memory>
#include <set>
#include <system_error>

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetSelect.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/Ret.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;
using namespace stone::driver;

using namespace llvm::opt;

static void SetInstallDir(llvm::ArrayRef<const char *> &argv, Driver &driver,
                          bool canonicalPrefixes) {
  // Attempt to find the original path used to invoke the driver, to determine
  // the installed path. We do this manually, because we want to support that
  // path being a symlink.
  llvm::SmallString<128> InstalledPath(argv[0]);

  // Do a PATH lookup, if there are no directory components.
  if (llvm::sys::path::filename(InstalledPath) == InstalledPath) {
    if (llvm::ErrorOr<std::string> Tmp = llvm::sys::findProgramByName(
            llvm::sys::path::filename(InstalledPath.str()))) {
      InstalledPath = *Tmp;
    }
  }

  // FIXME: We don't actually canonicalize this, we just make it absolute.
  if (canonicalPrefixes) {
    llvm::sys::fs::make_absolute(InstalledPath);
  }

  llvm::StringRef InstalledPathParent(
      llvm::sys::path::parent_path(InstalledPath));
  if (llvm::sys::fs::exists(InstalledPathParent)) {
    driver.SetInstalledDir(InstalledPathParent);
  }
}

int stone::Run(llvm::ArrayRef<const char *> args, const char *arg0,
               void *mainAddr) {
  auto driverPath = stone::GetExecutablePath(arg0);
  auto driverName = llvm::sys::path::stem(arg0);

  Driver driver(driverPath, driverName);

  bool canonicalPrefixes = false;

  SetInstallDir(args, driver, canonicalPrefixes);

  if (driver.Build(args)) {
    driver.Run();
    driver.Finish();
    if (driver.GetDiagEngine().HasError()) {
      return ret::err;
    }
  }
  return ret::ok;
}
