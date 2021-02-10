set(LLVM_LINK_COMPONENTS
  BitReader
  BitWriter
  Core
  Support
  Target
)
set(stone_session_sources
	Session.cpp
	SessionOptions.cpp

	LINK_LIBS 
	StoneUtils
)
add_stone_library(StoneSession
	${stone_session_sources}

	DEPENDS
	StoneOptions
)
