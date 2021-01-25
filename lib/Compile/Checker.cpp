#include "stone/Compile/Checker.h"

#include "stone/Core/Ret.h"

using namespace stone;
using namespace stone::semantics;
using namespace stone::syntax;

Checker::Checker(SourceUnit &su, Pipeline *pipeline)
    : su(su), pipeline(pipeline), stats(*this) {}

// Decl
void Checker::CheckDecl() {}

// Stmt
void Checker::CheckStmt() {}

// Expr
void Checker::CheckExpr() {}

void CheckerStats::Print() const {}
