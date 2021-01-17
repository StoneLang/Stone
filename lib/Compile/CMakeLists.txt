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
	Analysis.cpp
	Backend.cpp
	Check.cpp
	Checker.cpp
	Compile.cpp
	Compiler.cpp
	Gen.cpp
	Lexer.cpp
	Optimize.cpp
	Parse.cpp
	Parser.cpp
	Transformer.cpp
	
	LINK_LIBS
	StoneCore
	StoneSession
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



