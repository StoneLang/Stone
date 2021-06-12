#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
} // namespace llvm

namespace stone {
class Compiler;

namespace sys {
class CompilingScope final {
public:
  CompilingScope(const CompilingScope &) = delete;
  CompilingScope(CompilingScope &&) = delete;
  CompilingScope &operator=(const CompilingScope &) = delete;
  CompilingScope &operator=(CompilingScope &&) = delete;

public:
  CompilingScope() {}
  ~CompilingScope() {}

public:
  void Enter();
  void Exit();
};

class Frontend final {
  Compiler &compiler;

  // TODO: prevScope,
  CompilingScope scope;
  llvm::Module *llvmModule = nullptr;

private:
  Frontend(const Frontend &) = delete;
  Frontend(Frontend &&) = delete;
  Frontend &operator=(const Frontend &) = delete;
  Frontend &operator=(Frontend &&) = delete;

public:
  Frontend(Compiler &compiler) : compiler(compiler) {}
  ~Frontend();

public:
  Compiler &GetCompiler() { return compiler; }
  CompilingScope &GetScope() { return scope; }
  void SetLLVMModule(llvm::Module *m) { llvmModule = m; }

  llvm::Module *GetLLVMModule() {
    assert(llvmModule && "No LLVM Module");
    return llvmModule;
  }
};

int Parse(Frontend &fe);
int Parse(Frontend &fe, bool check);
int Check(Frontend &fe);
int EmitIR(Frontend &fe);
int EmitObject(Frontend &fe);
int EmitAssembly(Frontend &fe);
int EmitLibrary(Frontend &fe);
int EmitModuleOnly(Frontend &fe);
int EmitBitCode(Frontend &fe);

} // namespace sys
} // namespace stone
#endif
