#include "stone/Analyze/Parser.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Basic.h"
#include "stone/Basic/Ret.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Syntax/Scope.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(SourceModuleFile &sf, Syntax &syntax, ParserPipeline *pipeline)
    : Parser(sf, syntax,
             std::unique_ptr<Lexer>(
                 new Lexer(sf.GetSrcID(), syntax.GetTreeContext().GetSrcMgr(),
                           syntax.GetTreeContext().GetBasic()))) {}

Parser::Parser(SourceModuleFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lx,
               ParserPipeline *pipeline)
    : sf(sf), syntax(syntax), lexer(lx.release()), curDC(&sf),
      pipeline(pipeline) {

  stats.reset(new ParserStats(*this, GetBasic()));
  GetBasic().GetStatEngine().Register(stats.get());

  Init();
}
void Parser::Init() {

  // Create the translation unit scope.  Install it as the current scope.
  // assert(GetCurScope() == nullptr && "A scope is already active?");
  // Prime the Lexer
  ConsumeTok();
}

syn::Scope *Parser::GetCurScope() const { return nullptr; }
Parser::~Parser() {}

bool Parser::HasError() { return GetBasic().HasError(); }
Basic &Parser::GetBasic() { return syntax.GetTreeContext().GetBasic(); }

void Parser::EnterScope(unsigned scopeFlags) {}
void Parser::ExitScope() {}

SrcLoc Parser::ConsumeTok(bool onTok) {
  SrcLoc loc = tok.GetLoc();
  assert(tok.IsNot(tk::Type::eof) && "Lexing past eof!");
  Lex(tok, leadingTrivia, trailingTrivia);
  prevTokLoc = loc;
  return loc;
}

static bool HasFlagsSet(Parser::SkipToFlags L, Parser::SkipToFlags R) {
  return (static_cast<unsigned>(L) & static_cast<unsigned>(R)) != 0;
}
bool Parser::SkipTo(llvm::ArrayRef<tk::Type> toks, SkipToFlags flags) {
  // We always want this function to skip at least one token if the first token
  // isn't T and if not at EOF.
  bool isFirstTokenSkipped = true;
  while (true) {
    // If we found one of the tokens, stop and return true.
    for (unsigned i = 0, numToks = toks.size(); i != numToks; ++i) {
      if (tok.Is(toks[i])) {
        if (HasFlagsSet(flags, StopBeforeMatch)) {
          // Noop, don't consume the token.
        } else {
          ConsumeAnyTok();
        }
        return true;
      }
    }
    return false;
  }
}

void ParserStats::Print() {}
