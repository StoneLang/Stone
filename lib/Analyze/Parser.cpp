#include "stone/Analyze/Parser.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/Ret.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(const SrcID srcID, SourceFile &sf, SrcMgr &sm, Context &ctx,
               ParserPipeline *pipeline)
    : Parser(srcID, sf, sm, ctx,
             std::unique_ptr<Lexer>(new Lexer(srcID, sm, ctx))) {}

Parser::Parser(const SrcID srcID, SourceFile &sf, SrcMgr &sm, Context &ctx,
               std::unique_ptr<Lexer> lx, ParserPipeline *pipeline)
    : srcID(srcID), sf(sf), sm(sm), ctx(ctx), lexer(lx.release()),
      pipeline(pipeline) {

  stats.reset(new ParserStats(*this));
  ctx.GetStatEngine().Register(stats.get());

  // Populate the curTok
  // ConsumeToken();
}

Parser::~Parser() {}

void ParserStats::Print() {}
