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
set(stone_gen_sources
	Gen.cpp
	CodeGenBlocks.cpp
	CodeGenerator.cpp
	CodeEmitter.cpp
	CodeGenFunction.cpp
	Optimize.cpp

	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



