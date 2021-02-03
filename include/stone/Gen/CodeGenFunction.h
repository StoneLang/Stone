#ifndef STONE_GEN_CODEGENFUNCTION_H
#define STONE_GEN_CODEGENFUNCTION_H

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
}  // namespace llvm

namespace stone {
namespace gen {

class CodeGenFunction {
 public:
  CodeGenFunction();
};

}  // namespace gen
}  // namespace stone
#endif
