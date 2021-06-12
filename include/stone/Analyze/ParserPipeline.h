#ifndef STONE_ANALYZE_PARSERPIPELINE_H
#define STONE_ANALYZE_PARSERPIPELINE_H

#include "stone/Basic/PipelineEngine.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Stmt.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class ParserPipeline : public Pipeline {
public:
  llvm::StringRef GetName() override { return "ParserPipeline"; }

public:
  virtual void OnDeclParsed(const syn::Decl &decl) = 0;
  virtual void OnStmtParsed(const syn::Stmt &stmt) = 0;
  virtual void OnExprParsed(const syn::Expr &expr) = 0;
};
} // namespace stone
#endif
