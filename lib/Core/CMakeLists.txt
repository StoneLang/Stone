set(LLVM_LINK_COMPONENTS
  BinaryFormat
  Core
  Support
  )

set(stone_core_sources
	ASTContext.cpp
	ASTConsumer.cpp
	ASTScope.cpp
	ASTVisitor.cpp
	ASTWalker.cpp
	Builtin.cpp
	BumpTable.cpp
	Char.cpp
	Comment.cpp
	Context.cpp
	Decl.cpp
	Diagnostics.cpp
	Expr.cpp
	FileMgr.cpp
	FileSystemStatCache.cpp
	Fmt.cpp
	Identifier.cpp
	LLVMContext.cpp
	Module.cpp
	SrcLoc.cpp
	SrcMgr.cpp
	Stats.cpp
	Stmt.cpp
	Template.cpp
	TextDiagnosticListener.cpp
	TokenKind.cpp
	Type.cpp
	Version.cpp
	VirtualTable.cpp

)
add_stone_library(StoneCore
	${stone_core_sources}
)


