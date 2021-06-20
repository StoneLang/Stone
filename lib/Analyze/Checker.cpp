#include "stone/Analyze/Checker.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::sema;
using namespace stone::syn;

Checker::Checker(SourceModuleFile &sf, Syntax &syntax,
                 CheckerPipeline *pipeline)
    : sf(sf), syntax(syntax), pipeline(pipeline) {
  stats.reset(new CheckerStats(*this, syntax.GetTreeContext().GetContext()));
  syntax.GetTreeContext().GetContext().GetStatEngine().Register(stats.get());
}
// Decl
void Checker::CheckDecl() {}

// Stmt
void Checker::CheckStmt() {}

// Expr
void Checker::CheckExpr() {}

void CheckerStats::Print() {}
