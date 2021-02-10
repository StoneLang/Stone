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
	CompilerUnit.cpp

	LINK_LIBS
	StoneAnalyze
	StoneGen
	StoneSession

	DEPENDS
	StoneOptions

)
add_stone_library(StoneCompile
	${stone_compile_sources}
)



