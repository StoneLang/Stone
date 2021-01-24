#include "stone/Compile/Parser.h"

#include "stone/Core/Ret.h"

using namespace stone;
using namespace stone::syntax;

Parser::Parser(CompilePipeline *pipeline) : pipeline(pipeline), stats(*this) {}

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