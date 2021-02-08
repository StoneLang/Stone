#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

namespace stone {
namespace syn {
class FunDecl;
class StructDecl;

class IfStmt;

class Syntax final {
public:
  static FunDecl *CreateFunDecl();
  static StructDecl *CreateStructDecl();

public:
  static IfStmt *CreateIfStmt();
};
} // namespace syn
} // namespace stone
#endif
