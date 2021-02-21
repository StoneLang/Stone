#include "stone/Compile/Checker.h"
#include "stone/Utils/Ret.h"

using namespace stone;
using namespace stone::sema;
using namespace stone::syn;

Checker::Checker(SourceFile &su, Context &ctx, Pipeline *pipeline)
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
