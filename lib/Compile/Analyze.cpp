#include "stone/Compile/Analyze.h"

#include "stone/Compile/Checker.h"
#include "stone/Compile/Parser.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone::syntax;

void stone::ParseSourceUnit(SourceUnit &su, Pipeline *pipeline) {
  // Parser parser(pipeline);
}

void stone::CheckSourceUnit(SourceUnit &su, Pipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void stone::CheckWholeModule(SourceUnit &su, Pipeline *pipeline) {}
