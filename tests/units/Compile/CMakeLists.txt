set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(StoneCompileUnitTests
	CompileTest.cpp
)
target_link_libraries(StoneCompileUnitTests
  PRIVATE
	StoneCompile
)

