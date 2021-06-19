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

// Ex: sample.stone
// fun F0() -> void {}
// fun F1() -> void {}
// There are two top decls - F0 and F1
// This call parses one at a time and adds it to the SourceModuleFile
bool Parser::ParseTopDecl(DeclGroupPtrTy &result, bool isFirstDecl) {

  return false;
}

SyntaxResult<Decl *> Parser::ParseDecl() {

  if (pipeline) {
    // pipeline->OnDeclParsed();
  }

  return DeclEmpty();
}