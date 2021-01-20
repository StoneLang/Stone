#ifndef STONE_COMPILE_SYSTEM_H
#define STONE_COMPILE_SYSTEM_H

#include "stone/Compile/Analysis.h"
#include "stone/Compile/CompileOptions.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Session/Session.h"

using namespace stone::syntax;
using namespace stone::analysis;

namespace llvm {
class Module;
}
namespace stone {

class System final {
  Compiler &compiler;
  llvm::Module *llvmModule = nullptr;

 public:
  System(Compiler &compiler) : compiler(compiler) {}

 private:
  int Parse(bool check);
  int CheckSourceUnit(syntax::SourceUnit *su);
  int CheckModule(syntax::Module *m);

 public:
  int Parse();
  int Check();
  int EmitIR();
  int EmitObject();
  int EmitAssembly();
};

}  // namespace stone
#endif
