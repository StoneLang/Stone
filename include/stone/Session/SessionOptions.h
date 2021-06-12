#ifndef STONE_SESSION_SESSIONOPTIONS_H
#define STONE_SESSION_SESSIONOPTIONS_H

#include "stone/Basic/File.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Session/ModeType.h"

namespace llvm {
namespace opt {
class OptTable;
}
} // namespace llvm

namespace stone {

namespace opts {
enum OptFlag {
  CompilerOption = (1 << 4),
  NoCompilerOption = (1 << 5),
  DriverOption = (1 << 6),
  NoDriverOption = (1 << 7),
  DebugOption = (1 << 8),
};

enum OptID : unsigned {
  INVALID = 0, // This is not an option ID.
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM,  \
               HELPTEXT, METAVAR, VALUES)                                      \
  ID,
#include "stone/Session/SessionOptions.inc"
  LAST
#undef OPTION
};

std::unique_ptr<llvm::opt::OptTable> CreateOptTable();

} // namespace opts

class SessionOptions {
  std::unique_ptr<llvm::opt::OptTable> optTable;

public:
  bool printHelp = false;
  bool printHelpHidden = false;
  bool printVersion = false;
  bool printLifecycle = false;
  bool printStats = false;

  /// The default mode
  ModeType modeKind = ModeType::None;
  /// The name of the module
  llvm::StringRef moduleName;

  /// The file input kind
  file::Type inputType = file::Type::None;

  file::Files inputs;

public:
  SessionOptions() : optTable(stone::opts::CreateOptTable()) {}

public:
  llvm::opt::OptTable &GetOpts() const { return *optTable.get(); }

  void AddInput(file::Type ty, llvm::StringRef name) {
    inputs.push_back(std::make_pair(ty, name));
  }
};
} // namespace stone

#endif
