#include "stone/Syntax/SyntaxBuilder.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

// Decl
DeclSyntaxBuilder::DeclSyntaxBuilder(Syntax &syntax) : SyntaxBuilder(syntax) {}

// Block
BlockStmtSyntaxBuilder::BlockStmtSyntaxBuilder(Syntax &syntax)
    : SyntaxBuilder(syntax) {}

// Fun
FunDeclSyntaxBuilder::FunDeclSyntaxBuilder(Syntax &syntax)
    : DeclSyntaxBuilder(syntax), BlockStmtSyntaxBuilder(syntax) {}

void FunDeclSyntaxBuilder::WithFunKeyword() {}

// Struct
StructDeclSyntaxBuilder::StructDeclSyntaxBuilder(Syntax &syntax)
    : DeclSyntaxBuilder(syntax), BlockStmtSyntaxBuilder(syntax) {}

void StructDeclSyntaxBuilder::WithStructKeyword() {}