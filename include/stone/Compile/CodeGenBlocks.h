#ifndef STONE_COMPILE_CODEGENBLOCKS_H
#define STONE_COMPILE_CODEGENBLOCKS_H

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

class CodeGenBlocks {
 public:
  CodeGenBlocks();
};

}  // namespace gen
}  // namespace stone
#endif
