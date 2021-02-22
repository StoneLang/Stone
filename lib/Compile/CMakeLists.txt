set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  Option
  )
set(stone_compile_sources
	Compile.cpp
	Compiler.cpp
	CompilerScope.cpp
	Check.cpp
	CheckDecl.cpp
	Checker.cpp
	CheckExpr.cpp
	CheckStmt.cpp
	Lexer.cpp
	Optimize.cpp
	Parse.cpp
	ParseDecl.cpp
	Parser.cpp
	ParseExpr.cpp
	ParseStmt.cpp
	Serialize.cpp

	LINK_LIBS
	StoneGen
	StoneSession

	DEPENDS
	StoneOptions
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



