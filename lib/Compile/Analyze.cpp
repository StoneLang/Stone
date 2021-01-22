#include "stone/Compile/Analyze.h"

#include "stone/Compile/Analysis.h"
#include "stone/Compile/Checker.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone::syntax;

int stone::ParseSourceUnit(SourceUnit &su, CompilePipeline *pipeline) {
  return ret::ok;
}

int stone::CheckSourceUnit(SourceUnit &su, CompilePipeline *pipeline) {
  return ret::ok;
}

int stone::CheckWholeModule(SourceUnit &su, CompilePipeline *pipeline) {
  // Checker checker;
  return ret::ok;
}
