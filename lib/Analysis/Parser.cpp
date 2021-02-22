#include "stone/Analysis/Parser.h"
#include "stone/Utils/Ret.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(Context &ctx, Pipeline *pipeline)
    : ctx(ctx), pipeline(pipeline) {
  stats.reset(new ParserStats(*this));
  ctx.GetStatEngine().Register(stats.get());
  timer.startTimer();
}

Parser::~Parser() {}

void ParserStats::Print() {}
