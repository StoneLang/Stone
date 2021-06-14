#include "stone/Analyze/Parse.h"
#include "stone/Analyze/LexerPipeline.h"
#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserPipeline.h"

#include "stone/Basic/Ret.h"
#include "stone/Public.h"
#include "stone/Syntax/Module.h"

using namespace stone::syn;

void stone::ParseSourceModuleFile(const SrcID srcID, SourceModuleFile &sf,
                                  SrcMgr &sm, Context &ctx,
                                  PipelineEngine *pe) {

  ParserPipeline *parserPipeline = nullptr;
  LexerPipeline *lexerPipeline = nullptr;

  if (pe) {
    parserPipeline =
        static_cast<ParserPipeline *>(pe->Get(PipelineType::Parse));

    lexerPipeline = static_cast<LexerPipeline *>(pe->Get(PipelineType::Lex));
  }
  Parser parser(srcID, sf, sm, ctx, parserPipeline);
  if (lexerPipeline) {
    parser.GetLexer().SetPipeline(lexerPipeline);
  }

  // TODO: Error is another condition to
  while (!parser.IsDone()) {
    // check for errors from diag, if there are exit.
    // Go through all of the top level decls in the file
    parser.ParseTopDecl();
  }
}
