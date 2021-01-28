#include "stone/Compile/Parser.h"

#include "stone/Core/Ret.h"

using namespace stone;
using namespace stone::syntax;

Parser::Parser(Context& ctx, Pipeline* pipeline)
    : ctx(ctx), pipeline(pipeline) {
  stats.reset(new ParserStats("Parser", *this));
  ctx.GetStatEngine().Register(stats.get());
}

Parser::~Parser() {}
//===----------------------------------------------------------------------===//
// Decl
//===----------------------------------------------------------------------===//
int Parser::ParseTopDecl() { return ret::ok; }

void Parser::ParseDecl() {}
//===----------------------------------------------------------------------===//
// Stmt
//===----------------------------------------------------------------------===//

void Parser::ParseStmt() {}

//===----------------------------------------------------------------------===//
// Expr
//===----------------------------------------------------------------------===//
void Parser::ParseExpr() {}

void ParserStats::Print() const {}
