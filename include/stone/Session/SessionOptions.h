#ifndef STONE_SESSION_SESSIONOPTIONS_H
#define STONE_SESSION_SESSIONOPTIONS_H

#include "stone/Core/LangOptions.h"
#include "stone/Session/FileType.h"
#include "stone/Session/ModeKind.h"
#include "stone/Session/Options.h"

namespace stone {

class SessionOptions {
  std::unique_ptr<llvm::opt::OptTable> optTable;

 public:
  bool showHelp = false;
  bool showHidden = false;
  bool showVersion = false;
  bool showLifecycle = false;
  ModeKind modeKind = ModeKind::None;
  /// The name of the module
  llvm::StringRef moduleName;

  /// The file input kind
  file::FileType inputFileType = file::FileType::Stone;

 public:
  SessionOptions() : optTable(stone::CreateOptTable()) {}

 public:
  llvm::opt::OptTable &GetOpts() const { return *optTable.get(); }
};
}  // namespace stone

#endif
