set(LLVM_LINK_COMPONENTS
  BitReader
  BitWriter
  Core
  Support
  Target
)
set(stone_session_sources
	FileType.cpp
	Options.cpp
	Session.cpp

	LINK_LIBS
	StoneCore
)
add_stone_library(StoneSession
	${stone_session_sources}

	DEPENDS
	StoneOptions
)
