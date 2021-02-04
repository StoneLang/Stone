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
set(stone_parse_sources
	Lexer.cpp
	LexerDiagnostics.cpp
	Parse.cpp
	Parser.cpp
	ParserDiagnostics.cpp

	LINK_LIBS
	StoneCore

)
add_stone_library(StoneParse
	${stone_parse_sources}
)



