#include "stone/Analyze/Parser.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Basic.h"
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
                           syntax.GetTreeContext().GetBasic()))) {}

Parser::Parser(SourceModuleFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lx,
               ParserPipeline *pipeline)
    : sf(sf), syntax(syntax), lexer(lx.release()), curDC(&sf),
      pipeline(pipeline) {

  stats.reset(new ParserStats(*this, GetBasic()));
  GetBasic().GetStatEngine().Register(stats.get());
}
Parser::~Parser() {}

bool Parser::HasError() { return GetBasic().HasError(); }
Basic &Parser::GetBasic() { return syntax.GetTreeContext().GetBasic(); }

void Parser::EnterScope(unsigned scopeFlags) {}
void Parser::ExitScope() {}

SrcLoc Parser::ConsumeTok(bool onTok) {
  SrcLoc loc = tok.GetLoc();
  assert(tok.IsNot(tk::Type::eof) && "Lexing past eof!");
  Lex(tok, leadingTrivia, trailingTrivia);
  prevTokLoc = loc;
  return loc;
}

void ParserStats::Print() {}
