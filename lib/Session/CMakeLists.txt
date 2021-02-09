set(LLVM_LINK_COMPONENTS
  BitReader
  BitWriter
  Core
  Support
  Target
)
set(stone_session_sources
	Options.cpp
	Session.cpp

	LINK_LIBS 
	StoneUtils
)
add_stone_library(StoneSession
	${stone_session_sources}

	DEPENDS
	StoneOptions
)
