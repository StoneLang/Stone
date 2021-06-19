#include "stone/Analyze/Parser.h"
#include "stone/Analyze/SyntaxResult.h"

using namespace stone::syn;

SyntaxResult<Expr *> Parser::ParseExpr() { return ExprEmpty(); }
