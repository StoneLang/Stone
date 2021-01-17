#ifndef STONE_SESSION_OPTIONS_H
#define STONE_SESSION_OPTIONS_H

#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "stone/Core/Mem.h"

namespace llvm {
namespace opt {
class OptTable;
}
}  // namespace llvm
namespace stone {
namespace opts {

enum OptFlag {
  CompileOption = (1 << 4),
  NoCompileOption = (1 << 5),
  DriverOption = (1 << 6),
  NoDriverOption = (1 << 7),
  DebugOption = (1 << 8),
};

enum OptID : unsigned {
  INVALID = 0,  // This is not an option ID.
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM, \
               HELPTEXT, METAVAR, VALUES)                                     \
  ID,
#include "stone/System/Options.inc"
  LAST
#undef OPTION
};
}  // namespace opts
std::unique_ptr<llvm::opt::OptTable> CreateOptTable();
}  // namespace stone

#endif
