#include "stone/Analyze/Parser.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Ret.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(Context &ctx, ParserPipeline *pipeline)
    : ctx(ctx), pipeline(pipeline) {
  stats.reset(new ParserStats(*this));
  ctx.GetStatEngine().Register(stats.get());
}

Parser::~Parser() {}

void ParserStats::Print() {}
