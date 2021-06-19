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

  ParserPipeline *pp = nullptr;
  LexerPipeline *lp = nullptr;

  if (pe) {
    pp = static_cast<ParserPipeline *>(pe->Get(PipelineType::Parse));
    lp = static_cast<LexerPipeline *>(pe->Get(PipelineType::Lex));
  }
  // TODO: Since we have the sf, we do not need to pass SrcID
  Parser parser(sf, sm, ctx, pp);
  if (lp) {
    parser.GetLexer().SetPipeline(lp);
  }

  // TODO: Error is another condition to
  while (true) {

    // Check for tk::eof
    if (parser.IsDone()) {
      pp->OnDone();
      break;
    }

    // Check for errors from diag and if there are then exit.
    if (ctx.Error()) {
      pp->OnError();
      break;
    }

    // Go through all of the top level decls in the file one at a time
    // As you process a decl, it will be added to the SourceModuleFile
    if (parser.ParseTopDecl()) {

      // Notifify that a top decl has been parsed.
      pp->OnTopDecl(nullptr); // TODO: Pass null for now.
    }
  }
}
