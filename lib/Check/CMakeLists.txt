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
	CheckerDiagnostics.cpp

	LINK_LIBS
	StoneAST
)
add_stone_library(StoneCheck
	${stone_check_sources}
)



