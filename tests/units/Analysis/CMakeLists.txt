set(LLVM_LINK_COMPONENTS
  Core
  Support
)
add_stone_unittest(StoneAnalysisTests
	LexerTest.cpp
	ParserTest.cpp
)
target_link_libraries(StoneAnalysisTests
  PRIVATE
	StoneAnalysis
)

