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
	DarwinDirWatcher.cpp
	DiagnosticEngine.cpp
	DirScanner.cpp
	DirWatcher.cpp
	FileMgr.cpp
	FileSystemStatCache.cpp
	Fmt.cpp
	LLVMContext.cpp
	SrcLoc.cpp
	SrcMgr.cpp
	Stats.cpp
	TextDiagnosticListener.cpp
	TokenKind.cpp
	UnixDirWatcher.cpp
	Version.cpp
	WinDirWatcher.cpp
)
add_stone_library(StoneUtils
	${stone_utils_sources}
)



