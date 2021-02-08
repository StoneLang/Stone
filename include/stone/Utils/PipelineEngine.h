#ifndef STONE_UTILS_PIPELINEENGINE_H
#define STONE_UTILS_PIPELINEENGINE_H

#include <iostream>

#include "stone/Utils/Color.h"
#include "stone/Utils/List.h"
#include "stone/Utils/Mem.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Chrono.h"

namespace stone {

enum class PipelineType {
  Check,
  Parse,
  Lex,
  Gen,
};

class Pipeline {};
class CheckPipeline : public Pipeline {};
class ParsePipeline : public Pipeline {};
class LexPipeline : public Pipeline {};

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
