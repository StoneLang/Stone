set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  )
set(stone_analysis_sources
	Check.cpp
	Checker.cpp
	Lexer.cpp
	Parse.cpp
	Parser.cpp

	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneAnalysis
	${stone_analysis_sources}
)



