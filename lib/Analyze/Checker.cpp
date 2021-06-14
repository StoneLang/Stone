#include "stone/Analyze/Checker.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Ret.h"

using namespace stone;
using namespace stone::sema;
using namespace stone::syn;

Checker::Checker(SourceModuleFile &su, Context &ctx, CheckerPipeline *pipeline)
    : su(su), ctx(ctx), pipeline(pipeline) {
  stats.reset(new CheckerStats(*this));
  ctx.GetStatEngine().Register(stats.get());
}

// Decl
void Checker::CheckDecl() {}

// Stmt
void Checker::CheckStmt() {}

// Expr
void Checker::CheckExpr() {}

void CheckerStats::Print() {}
