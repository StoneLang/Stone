#include "stone/Analyze/Parse.h"
#include "stone/Analyze/LexerPipeline.h"
#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserPipeline.h"
#include "stone/Basic/Ret.h"
#include "stone/Public.h"
#include "stone/Syntax/Module.h"

using namespace stone::syn;

void stone::ParseSourceModuleFile(SourceModuleFile &sf, SrcMgr &sm,
                                  Context &ctx, PipelineEngine *pe) {

  ParserPipeline *parserPipeline = nullptr;
  LexerPipeline *lexerPipeline = nullptr;

  if (pe) {
    parserPipeline =
        static_cast<ParserPipeline *>(pe->Get(PipelineType::Parse));
    lexerPipeline = static_cast<LexerPipeline *>(pe->Get(PipelineType::Lex));
  }
  // TODO: Since we have the sf, we do not need to pass SrcID
  Parser parser(sf, sm, ctx, parserPipeline);
  if (lexerPipeline) {
    parser.GetLexer().SetPipeline(lexerPipeline);
  }

  // TODO: Error is another condition to
  while (!parser.IsDone() && !ctx.Error()) {
    // check for errors from diag, if there are exit.
    // Go through all of the top level decls in the file
    if (!parser.ParseTopDecl()) {
      break;
    }
  }
}
