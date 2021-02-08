#include "stone/Parse/Parser.h"
#include "stone/Utils/Ret.h"

using namespace stone;
using namespace stone::syn;
using namespace stone::analysis;

Parser::Parser(Context &ctx, Pipeline *pipeline)
    : ctx(ctx), pipeline(pipeline) {
  stats.reset(new ParserStats(*this));
  ctx.GetStatEngine().Register(stats.get());
  clock.Start();
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

void ParserStats::Print() {}
