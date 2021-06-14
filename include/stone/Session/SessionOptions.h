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

private:
  file::Files inputs;

public:
  SessionOptions() : optTable(stone::opts::CreateOptTable()) {}

public:
  llvm::opt::OptTable &GetOpts() const { return *optTable.get(); }

  void AddInput(llvm::StringRef name) {
    auto ty = file::GetTypeByName(name);
    assert(ty != file::Type::INVALID && "Invalid input type.");
    inputs.push_back(std::make_pair(ty, name));
  }
  void AddInput(llvm::StringRef name, file::Type ty) {
    inputs.push_back(std::make_pair(ty, name));
  }

  file::Files &GetInputs() { return inputs; }
};
} // namespace stone

#endif
