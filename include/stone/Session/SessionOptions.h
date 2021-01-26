#ifndef STONE_SESSION_SESSIONOPTIONS_H
#define STONE_SESSION_SESSIONOPTIONS_H

#include "stone/Core/LangOptions.h"
#include "stone/Session/FileType.h"
#include "stone/Session/ModeKind.h"
#include "stone/Session/Options.h"

namespace stone {

/// An input argument from the command line and its inferred type.
using InputFile = std::pair<file::FileType, const llvm::StringRef>;
/// Type used for a list of input arguments.
using InputFiles = llvm::SmallVector<InputFile, 16>;

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
  file::FileType inputFileType = file::FileType::None;

  InputFiles inputs;

 public:
  SessionOptions() : optTable(stone::CreateOptTable()) {}

 public:
  llvm::opt::OptTable &GetOpts() const { return *optTable.get(); }

  void AddInput(file::FileType ty, llvm::StringRef name) {
    inputs.push_back(std::make_pair(ty, name));
  }
};
}  // namespace stone

#endif
