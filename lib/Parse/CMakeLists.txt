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
	Parse.cpp
	Parser.cpp

	LINK_LIBS
	StoneSyntax

)
add_stone_library(StoneParse
	${stone_parse_sources}
)



