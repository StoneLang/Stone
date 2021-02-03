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
	CodeGenFunction.cpp
	CodeGenerator.cpp
	Emitter.cpp

	LINK_LIBS
	StoneCore
)
add_stone_library(StoneGen
	${stone_gen_sources}
)



