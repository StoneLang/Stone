#ifndef STONE_COMPILE_GENERATOR_H
#define STONE_COMPILE_GENERATOR_H

#include "stone/Compile/GenProfile.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Context.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/Module.h"

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
class Generator final {
 public:
  Generator();

 public:
  /// Return an opaque reference to the GenModuleProfile object, which can
  /// be used in various secondary APIs.  It is valid as long as the
  /// Generator exists.
  GenModuleProfile &GetGenModuleProfile();

  /// Return the module that this code generator is building into.
  ///
  /// This may return null after HandleTranslationUnit is called;
  /// this signifies that there was an error generating code.  A
  /// diagnostic will have been generated in this case, and the module
  /// will be deleted.
  ///
  /// It will also return null if the module is released.
  llvm::Module *GetModule();
};
}  // namespace gen
}  // namespace stone
#endif
