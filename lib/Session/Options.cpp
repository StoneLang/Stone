#include "stone/Session/Options.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"

using namespace stone::opts;
using namespace llvm::opt;

#define PREFIX(NAME, VALUE) static const char *const NAME[] = VALUE;
#include "stone/Session/Options.inc"
#undef PREFIX

static const OptTable::Info InfoTable[] = {
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM,  \
               HELPTEXT, METAVAR, VALUES)                                      \
  {PREFIX, NAME,  HELPTEXT, METAVAR, ID,        Option::KIND##Class,           \
   PARAM,  FLAGS, GROUP,    ALIAS,   ALIASARGS, VALUES},
#include "stone/Session/Options.inc"
#undef OPTION
};

namespace {
class StoneOptTable : public OptTable {
public:
  StoneOptTable() : OptTable(InfoTable) {}
};
} // end anonymous namespace

std::unique_ptr<OptTable> stone::CreateOptTable() {
  return std::unique_ptr<OptTable>(new StoneOptTable());
}
