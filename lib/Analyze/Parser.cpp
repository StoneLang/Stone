#include "stone/Analyze/Parser.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/Ret.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Syntax/Scope.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(SourceModuleFile &sf, Syntax &syntax, ParserPipeline *pipeline)
    : Parser(sf, syntax,
             std::unique_ptr<Lexer>(
                 new Lexer(sf.GetSrcID(), syntax.GetTreeContext().GetSrcMgr(),
                           syntax.GetTreeContext().GetContext()))) {}

Parser::Parser(SourceModuleFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lx,
               ParserPipeline *pipeline)
    : sf(sf), syntax(syntax), lexer(lx.release()), pipeline(pipeline) {

  stats.reset(new ParserStats(*this, syntax.GetTreeContext().GetContext()));
  syntax.GetTreeContext().GetContext().GetStatEngine().Register(stats.get());

  Prime();
}
Parser::~Parser() {}

void Parser::Prime() { assert(true && "TODO"); }

bool Parser::Error() { return syntax.GetTreeContext().GetContext().Error(); }

void Parser::EnterScope(unsigned scopeFlags) {}
void Parser::ExitScope() {}

void ParserStats::Print() {}
