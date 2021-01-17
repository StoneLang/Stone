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

set(stone_driver_sources

	Build.cpp
	Compilation.cpp
	DarwinToolChain.cpp
	Driver.cpp
	Activity.cpp
	Job.cpp
	JobQueue.cpp
	Run.cpp
	ToolChain.cpp
	UnixToolChain.cpp
	WinToolChain.cpp

	LINK_LIBS
	StoneCore
	StoneSession
)
add_stone_library(StoneDriver
	${stone_driver_sources}

	DEPENDS
	StoneOptions
)



