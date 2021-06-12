#include "stone/Analyze/Parse.h"
#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserPipeline.h"

#include "stone/Basic/Ret.h"
#include "stone/Public.h"
#include "stone/Syntax/Module.h"

using namespace stone::syn;

void stone::ParseSourceFile(syn::SourceFile &sf, PipelineEngine *pe) {

  ParserPipeline *parserPipeline = nullptr;
  if (pe) {
    parserPipeline =
        static_cast<ParserPipeline *>(pe->Get(PipelineType::Parse));
  }
  // Parser parser(pipeline);
}
