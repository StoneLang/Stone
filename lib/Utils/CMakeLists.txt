set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
	BitReader
	BinaryFormat
  BitstreamReader
  Support
	Core
  )
set(stone_utils_sources
	BumpTable.cpp
	Char.cpp
	Clock.cpp
	Context.cpp
	Diagnostics.cpp
	DirWatcher.cpp
	FileMgr.cpp
	FileSystemStatCache.cpp
	Fmt.cpp
	LLVMContext.cpp
	SrcLoc.cpp
	SrcMgr.cpp
	Stats.cpp
	TextDiagnosticListener.cpp
	Version.cpp
)
add_stone_library(StoneUtils
	${stone_utils_sources}
)



