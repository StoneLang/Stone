set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  )
set(stone_frontend_sources
	Check.cpp
	Checker.cpp
	FrontendInputFile.cpp
	Lexer.cpp
	Parse.cpp
	Parser.cpp

	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneFrontend
	${stone_frontend_sources}
)



