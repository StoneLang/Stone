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
  ParserPipeline() : Pipeline(PipelineType::Parse) {}

public:
  llvm::StringRef GetName() override { return "Parse"; }

public:
  virtual void OnDone() = 0;
  virtual void OnError() = 0;

public:
  virtual void OnTopDecl(const syn::Decl *decl) = 0;
  virtual void OnDecl(const syn::Decl *decl) = 0;

  virtual void OnStmt(const syn::Stmt *stmt) = 0;
  virtual void OnExpr(const syn::Expr *expr) = 0;
};
} // namespace stone
#endif
