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
	Check.cpp
	Checker.cpp
	Compile.cpp
	Compiler.cpp
	CompilerScope.cpp
	CompilerUnit.cpp
	Lexer.cpp
	Optimize.cpp
	Parse.cpp
	Parser.cpp
	
	LINK_LIBS
	StoneCore
	StoneGen
	StoneSession
)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



