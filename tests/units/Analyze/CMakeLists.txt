set(LLVM_LINK_COMPONENTS
  Core
  Support
)
add_stone_unittest(StoneAnalyzeTests
	LexerTest.cpp
	ParserTest.cpp
)
target_link_libraries(StoneAnalyzeTests
  PRIVATE
	StoneAnalyze
)

