#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserRAII.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/SyntaxResult.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsTopDecl(const Token &tok) {
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
bool Parser::ParseTopDecl(syn::DeclGroupPtrTy &result, bool isFirstDecl) {

  assert(IsTopDecl(tok) && "Invalid top-level declaration.");

  return false;
}

syn::DeclGroupPtrTy Parser::ParseDecl(ParsingDeclSpecifier *pds) {

  // DelimiterBalancer balancer(*this);
  // if (pds) {
  //   return ParseDeclImpl(*pds);
  // }
  // ParsingDeclSpecifier localDS(*this);
  // return ParseDeclImpl(localDS);

  return nullptr;
}
syn::DeclGroupPtrTy Parser::ParseDeclImpl(ParsingDeclSpecifier &pds,
                                          AccessLevel al) {

  // Decl *singleDecl = nullptr;
  // // SyntaxResult<Decl *> singleDecl;
  // switch (tok.GetType()) {
  // case tk::Type::kw_fun:
  //   singleDecl = ParseFunDecl(pds, al);
  //   break;
  // default:
  //   break;
  // }
  // return ToDeclGroup(singleDecl);
  return nullptr;
}

SyntaxResult<Decl *> Parser::ParseFunDecl(ParsingDeclSpecifier &pds,
                                          AccessLevel al) {

  assert(tok.GetType() == tk::Type::kw_fun &&
         "Attempting to parse a 'fun' decl with incorrect token.");

  return DeclEmpty();
}
