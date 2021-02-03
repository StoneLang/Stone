#ifndef STONE_COMPILE_EMITTER_H
#define STONE_COMPILE_EMITTER_H

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

class Emitter final {
 public:
  Emitter();

 public:
  void EmitStmt();

 public:
  void EmitDecl();

 public:
  void EmitExpr();
};
}  // namespace gen

}  // namespace stone
