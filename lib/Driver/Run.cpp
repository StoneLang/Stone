
#include "stone/Driver/Run.h"

#include <memory>
#include <set>
#include <system_error>

#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Ret.h"
#include "stone/Driver/Driver.h"
#include "stone/Session/ExecutablePath.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"

#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/StringSaver.h"

using namespace stone;
using namespace stone::driver;

using namespace llvm::opt;

static void SetInstallDir(const char *arg0, Driver &driver,
                          bool canonicalPrefixes) {
  // Attempt to find the original path used to invoke the driver, to determine
  // the installed path. We do this manually, because we want to support that
  // path being a symlink.
  llvm::SmallString<128> InstalledPath(arg0);

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

  // TODO: Move to session
  auto driverPath = stone::GetExecutablePath(arg0);
  auto driverName = llvm::sys::path::stem(arg0);

  Driver driver(driverPath, driverName);

  STONE_DEFER { driver.Finish(); };

  bool canonicalPrefixes = false;
  SetInstallDir(arg0, driver, canonicalPrefixes);

  driver.Init();
  if (driver.Build(args)) {
    if (driver.HasError()) {
      return ret::err;
    }
    driver.Run();
  }
  return ret::ok;
}
