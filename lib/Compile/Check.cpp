#include "stone/Compile/Analyze.h"
#include "stone/Compile/Checker.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone::syn;
using namespace stone::sema;

void stone::CheckSourceUnit(SourceUnit &su, Pipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void stone::CheckWholeModule(SourceUnit &su, Pipeline *pipeline) {
  // Checker checker
}
