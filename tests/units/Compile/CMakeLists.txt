set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(stoneAnalysisTests
	LexerTest.cpp
	ParserTest.cpp
)
target_link_libraries(stoneAnalysisTests
  PRIVATE
	stoneCore
	stoneAnalysis
)

