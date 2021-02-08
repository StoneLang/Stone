set(stone_include_files

	${STONE_SOURCE_DIR}/include/stone/Check/*.h
	${STONE_SOURCE_DIR}/include/stone/Compile/*.h
	#${STONE_SOURCE_DIR}/include/stone/Compile/*.def
	${STONE_SOURCE_DIR}/include/stone/Driver/*.h
	${STONE_SOURCE_DIR}/include/stone/Gen/*.h
	${STONE_SOURCE_DIR}/include/stone/Parse/*.h
	${STONE_SOURCE_DIR}/include/stone/Session/*.h
	${STONE_SOURCE_DIR}/include/stone/Syntax/*.h
	${STONE_SOURCE_DIR}/include/stone/Utils/*.h
	${STONE_SOURCE_DIR}/include/stone/*.h

)
set(stone_lib_files
	${STONE_SOURCE_DIR}/lib/Check/*.cpp
	${STONE_SOURCE_DIR}/lib/Compile/*.cpp
	${STONE_SOURCE_DIR}/lib/Driver/*.cpp
	${STONE_SOURCE_DIR}/lib/Gen/*.cpp
	${STONE_SOURCE_DIR}/lib/Parse/*.cpp
	${STONE_SOURCE_DIR}/lib/Session/*.cpp
	${STONE_SOURCE_DIR}/lib/Syntax/*.cpp
	${STONE_SOURCE_DIR}/lib/Utils/*.cpp
	#${STONE_SOURCE_DIR}/tests/units/Compile/*.cpp
	#${STONE_SOURCE_DIR}/tests/units/Core/*.cpp
	#${STONE_SOURCE_DIR}/tests/units/Driver/*.cpp

)
set(stone_tools_files
	${STONE_SOURCE_DIR}/tools/stone/*.cpp
	${STONE_SOURCE_DIR}/tools/try/*.cpp
)

find_program(CLANG_FORMAT clang-format)
	if(CLANG_FORMAT)
	add_custom_target(
		stone-fmt
		clang-format
		-i
		-style=llvm
		${stone_lib_files}
		${stone_include_files}
		${stone_tools_files}
	)
endif()
