set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
 )

set(stone_syn_sources
	Builtin.cpp
	Comment.cpp
	Decl.cpp
	DeclGroup.cpp
	Expr.cpp
	Identifier.cpp
	Module.cpp
	Stmt.cpp
	Scope.cpp
	Template.cpp
	TreeContext.cpp
	TreeListener.cpp
	Visitor.cpp
	Walker.cpp
	Type.cpp
	VirtualTable.cpp

	LINK_LIBS
	StoneBasic
)
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


