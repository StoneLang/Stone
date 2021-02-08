set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
  )

set(stone_syn_sources
	TreeContext.cpp
	TreeListener.cpp
	TreeScope.cpp
	TreeVisitor.cpp
	TreeWalker.cpp
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
add_stone_library(StoneSyntax
	${stone_syn_sources}
)


