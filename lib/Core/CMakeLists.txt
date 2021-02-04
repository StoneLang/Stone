set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
  )

set(stone_core_sources
	ASTContext.cpp
	ASTDiagnostics.cpp
	ASTListener.cpp
	ASTScope.cpp
	ASTVisitor.cpp
	ASTWalker.cpp
	Builtin.cpp
	Comment.cpp
	Context.cpp
	Decl.cpp
	Expr.cpp
	Identifier.cpp
	Module.cpp
	Stmt.cpp
	Template.cpp
	TokenKind.cpp
	Type.cpp
	VirtualTable.cpp

	LINK_LIBS
	StoneUtils

)
add_stone_library(StoneCore
	${stone_core_sources}
)


