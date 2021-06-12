#include "stone/Analyze/Parser.h"
#include "stone/Basic/Ret.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(Context &ctx, Pipeline *pipeline)
    : ctx(ctx), pipeline(pipeline) {
  stats.reset(new ParserStats(*this));
  ctx.GetStatEngine().Register(stats.get());
}

Parser::~Parser() {}

void ParserStats::Print() {}
