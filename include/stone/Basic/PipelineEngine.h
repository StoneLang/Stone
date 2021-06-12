#ifndef STONE_BASIC_PIPELINEENGINE_H
#define STONE_BASIC_PIPELINEENGINE_H

#include <iostream>

#include "stone/Basic/Color.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Mem.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"

namespace stone {

enum class PipelineType {
  Check,
  Parse,
  Lex,
  Gen,
};

class Pipeline {

public:
  Pipeline() = default;
  virtual ~Pipeline();

public:
  virtual llvm::StringRef GetName() = 0;
};

class PipelineEngine final {
public:
  PipelineEngine();
  ~PipelineEngine();

public:
  void Add(Pipeline *pipeline);
  Pipeline *Get(PipelineType ty);
};
} // namespace stone

#endif
