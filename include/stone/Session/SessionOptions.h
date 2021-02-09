#ifndef STONE_SESSION_SESSIONOPTIONS_H
#define STONE_SESSION_SESSIONOPTIONS_H

#include "stone/Session/ModeKind.h"
#include "stone/Session/Options.h"
#include "stone/Utils/InputFile.h"
#include "stone/Utils/LangOptions.h"

namespace stone {
class SessionOptions {
  std::unique_ptr<llvm::opt::OptTable> optTable;

public:
  bool printHelp = false;
  bool printHelpHidden = false;
  bool printVersion = false;
  bool printLifecycle = false;
  bool printStats = false;

  /// The default mode
  ModeKind modeKind = ModeKind::None;
  /// The name of the module
  llvm::StringRef moduleName;

  /// The file input kind
  file::Type inputType = file::Type::None;

  file::InputFiles inputs;

public:
  SessionOptions() : optTable(stone::CreateOptTable()) {}

public:
  llvm::opt::OptTable &GetOpts() const { return *optTable.get(); }

  void AddInput(file::Type ty, llvm::StringRef name) {
    inputs.push_back(std::make_pair(ty, name));
  }
};
} // namespace stone

#endif
