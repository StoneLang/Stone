set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  )
set(stone_analyze_sources
	Check.cpp
	CheckDecl.cpp
	Checker.cpp
	CheckExpr.cpp
	CheckStmt.cpp
	Lexer.cpp
	Parse.cpp
	ParseDecl.cpp
	Parser.cpp
	ParseExpr.cpp
	ParseStmt.cpp

	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneAnalyze
	${stone_analyze_sources}
)



