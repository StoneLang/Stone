#include "stone/Analyze/Parse.h"
#include "stone/Analyze/LexerPipeline.h"
#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserPipeline.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

using namespace stone::syn;

void stone::ParseSourceModuleFile(SourceModuleFile &sf, Syntax &syntax,
                                  PipelineEngine *pe) {

  ParserPipeline *pp = nullptr;
  LexerPipeline *lp = nullptr;

  Parser parser(sf, syntax);

  if (pe) {
    if (pe->Get(PipelineType::Parse)) {
      pp = static_cast<ParserPipeline *>(pe->Get(PipelineType::Parse));
      if (pp) {
        parser.SetPipeline(pp);
      }
    }
    if (pe->Get(PipelineType::Lex)) {
      lp = static_cast<LexerPipeline *>(pe->Get(PipelineType::Lex));
      if (lp) {
        parser.GetLexer().SetPipeline(lp);
      }
    }
  }

  syn::DeclGroupPtrTy topDecl;
  while (true) {
    // Check for tk::eof
    if (parser.IsDone()) {
      if (pp) {
        pp->OnDone();
      }
      break;
    }
    // Check for errors from diag and if there are then exit.
    if (parser.HasError()) {
      if (pp) {
        pp->OnError();
      }
      break;
    }
    // Go through all of the top level decls in the file one at a time
    // As you process a decl, it will be added to the SourceModuleFile
    if (parser.ParseTopDecl(topDecl)) {
      // Notifify that a top decl has been parsed.
      if (pp) {
        pp->OnTopDecl(topDecl.get().getSingleDecl());
      }
    }
  }
}
