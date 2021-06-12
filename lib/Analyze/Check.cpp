#include "stone/Analyze/Check.h"
#include "stone/Analyze/Checker.h"
#include "stone/Analyze/CheckerPipeline.h"

#include "stone/Basic/Ret.h"
#include "stone/Public.h"

using namespace stone::syn;
using namespace stone::sema;

void stone::CheckSourceFile(SourceFile &sf, CheckerPipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void stone::CheckWholeModule(SourceFile &sf, CheckerPipeline *pipeline) {
  // Checker checker
}
