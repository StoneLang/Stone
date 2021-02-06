set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
  )

set(stone_ast_sources
	ASTContext.cpp
	ASTDiagnostics.cpp
	ASTListener.cpp
	ASTScope.cpp
	ASTVisitor.cpp
	ASTWalker.cpp
	Builtin.cpp
	Comment.cpp
	Decl.cpp
	Expr.cpp
	Identifier.cpp
	Module.cpp
	Stmt.cpp
	Template.cpp
	Type.cpp
	VirtualTable.cpp

	LINK_LIBS
	StoneUtils
)
add_stone_library(StoneAST
	${stone_ast_sources}
)


