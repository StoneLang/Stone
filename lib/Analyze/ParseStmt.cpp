#include "stone/Analyze/Parser.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxResult.h"

using namespace stone::syn;

SyntaxResult<Stmt *> Parser::ParseStmt() { return StmtEmpty(); }
