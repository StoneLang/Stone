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
	AssembleJob.cpp
	BackendJob.cpp
	CmdOutput.cpp
	Compilation.cpp
	CompileJob.cpp
	CrashState.cpp
	DriverStats.cpp
	DarwinToolChain.cpp
	Driver.cpp
	Job.cpp
	JobBuilder.cpp
	LinkJob.cpp
	Run.cpp
	ToolChain.cpp
	TaskQueue.cpp
	UnixToolChain.cpp
	UnixTaskQueue.cpp
	WinToolChain.cpp
	WinTaskQueue.cpp

	LINK_LIBS
	StoneCore
	StoneSession
)
add_stone_library(StoneDriver
	${stone_driver_sources}

	DEPENDS
	StoneOptions
)



