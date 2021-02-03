set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
  Core
  )
set(stone_utils_sources
	DirWatcher.cpp
	LINK_LIBS
	StoneUtils
)
add_stone_library(StoneUtils
	${stone_utils_sources}
)



