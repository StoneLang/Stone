set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  IPO
  )
set(stone_check_sources
	Check.cpp
	Checker.cpp

	LINK_LIBS
	StoneSyntax
)
add_stone_library(StoneCheck
	${stone_check_sources}
)



