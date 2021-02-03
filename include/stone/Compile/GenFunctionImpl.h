#ifndef STONE_COMPILE_GENFUNCTIONIMPL_H
#define STONE_COMPILE_GENFUNCTIONIMPL_H

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

class GenFunctionImpl {
 public:
  GenFunctionImpl();
};

}  // namespace gen
}  // namespace stone
#endif
