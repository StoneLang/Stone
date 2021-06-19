#include "stone/Analyze/Parser.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/Ret.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Syntax/Scope.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(SourceModuleFile &sf, SrcMgr &sm, Context &ctx,
               ParserPipeline *pipeline)
    : Parser(sf, sm, ctx,
             std::unique_ptr<Lexer>(new Lexer(sf.GetSrcID(), sm, ctx))) {}

Parser::Parser(SourceModuleFile &sf, SrcMgr &sm, Context &ctx,
               std::unique_ptr<Lexer> lx, ParserPipeline *pipeline)
    : sf(sf), sm(sm), ctx(ctx), lexer(lx.release()), pipeline(pipeline) {

  stats.reset(new ParserStats(*this, ctx));
  ctx.GetStatEngine().Register(stats.get());

  // Populate the curTok
  // ConsumeToken();
}

Parser::~Parser() {}

void Parser::EnterScope(unsigned scopeFlags) {}
void Parser::ExitScope() {}

void ParserStats::Print() {}
