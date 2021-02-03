#ifndef STONE_GEN_CODEGENFUNCTION_H
#define STONE_GEN_CODEGENFUNCTION_H

#include "stone/Gen/CodeEmitter.h"

namespace stone {
namespace gen {

class CodeGenFunction final {
 public:
  CodeEmitter emitter;

 public:
  CodeEmitter& GetEmitter() { return emitter; }
};
}  // namespace gen
}  // namespace stone

#endif
