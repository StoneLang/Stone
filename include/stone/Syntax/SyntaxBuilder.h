#ifndef STONE_SYNTAX_SYNTAXBUILDER_H
#define STONE_SYNTAX_SYNTAXBUILDER_H

namespace stone {
namespace syn {

class Decl;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class BraceStmt; 

class SyntaxBuilder {
public:
};

class BlockStmtSyntaxBuilder : public SyntaxBuilder {
public:
  void WithLeftBrace();
  void WithRightBrace();
};
class FunDeclSyntaxBuilder : public BlockStmtSyntaxBuilder {
  //FunDecl *funDecl;

public:
  FunDeclSyntaxBuilder(const FunDeclSyntaxBuilder &) = delete;
  FunDeclSyntaxBuilder(FunDeclSyntaxBuilder &&) = delete;
  FunDeclSyntaxBuilder &operator=(const FunDeclSyntaxBuilder &) = delete;
  FunDeclSyntaxBuilder &operator=(FunDeclSyntaxBuilder &&) = delete;

public:
  void WithFunKeyword();
  void WithParams();
  void WithReturnType();
  void WithReturnStmt();

  // void WithAccessLevel(AccessLevel level);
};

class StructDeclSyntaxBuilder : public BlockStmtSyntaxBuilder {
public:
  void WithStructKeyword();
};

class ModuleDeclSyntaxBuilder final : public SyntaxBuilder {
public:
};

} // namespace syn
} // namespace stone

#endif
