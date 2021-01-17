set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_stone_unittest(stoneCoreTests
	BuiltinTest.cpp
  DiagTest.cpp
	FileMgrTest.cpp
	SrcMgrTest.cpp
)
target_link_libraries(stoneCoreTests
  PRIVATE
	stoneCore
)

