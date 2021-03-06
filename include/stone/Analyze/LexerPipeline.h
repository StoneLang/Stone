#ifndef STONE_ANALYZE_LEXERPIPELINE_H
#define STONE_ANALYZE_LEXERPIPELINE_H

#include "stone/Analyze/Token.h"
#include "stone/Basic/PipelineEngine.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class LexerPipeline : public Pipeline {
public:
  LexerPipeline() : Pipeline(PipelineType::Lex) {}

public:
  llvm::StringRef GetName() override { return "Lex"; }

public:
  virtual void OnTokenCreated(const syn::Token &token) = 0;
};

} // namespace stone
#endif
