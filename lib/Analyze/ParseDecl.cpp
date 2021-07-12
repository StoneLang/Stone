#include "stone/Analyze/Parser.h"
#include "stone/Analyze/ParserRAII.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsTopDecl(const Token &tok) {
  switch (tok.GetType()) {
  case tk::Type::kw_module:
  case tk::Type::kw_interface:
  case tk::Type::kw_fun:
  case tk::Type::kw_struct:
  case tk::Type::kw_space:
  case tk::Type::kw_const:
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

  ConsumeTok();

  return false;
}

syn::DeclGroupPtrTy Parser::ParseDecl(ParsingDeclSpecifier *pds) {

  PairDelimiterBalancer pairDelimiterBalancer(*this);
  // if (pds) {
  //   return ParseDecl(*pds);
  // }

  // ParsingDeclSpecifier localDS(*this);
  // return ParseDecl(localDS);

  return nullptr;
}
syn::DeclGroupPtrTy Parser::ParseDecl(ParsingDeclSpecifier &pds,
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

// void Parser::ParseFunctionPrototype() {

// }

SyntaxResult<Decl *> Parser::ParseFunDecl(ParsingDeclSpecifier &pds,
                                          AccessLevel al) {

  assert(tok.GetType() == tk::Type::kw_fun &&
         "Attempting to parse a 'fun' decl with incorrect token.");

  // ParseFunctionPrototype();

  return DeclEmpty();
}
