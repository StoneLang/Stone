#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserRAII.h"
#include "stone/Analyze/SyntaxResult.h"
#include "stone/Basic/Ret.h"

using namespace stone::syn;

bool Parser::IsDecl(const Token &tok) {
  switch (tok.GetType()) {
  case tk::Type::kw_fun:
    return true;
  default:
    return false;
  }
}

bool Parser::ParseTopDecl(DeclGroupPtrTy &result, bool isFirstDecl) {

  while (!IsDone()) {

    ParseDecl();
  }
  return false;
}

SyntaxResult<Decl *> Parser::ParseDecl() {

  if (pipeline) {
    // pipeline->OnDeclParsed();
  }

  return DeclEmpty();
}