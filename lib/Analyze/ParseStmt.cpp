#include "stone/Analyze/Parser.h"
#include "stone/Analyze/SyntaxResult.h"

using namespace stone::syn;

SyntaxResult<Stmt *> Parser::ParseStmt() { return StmtEmpty(); }
